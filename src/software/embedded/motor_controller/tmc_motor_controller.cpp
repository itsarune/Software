#include "software/embedded/motor_controller/tmc_motor_controller.h"

#include "software/logger/logger.h"

extern "C"
{
#include "external/trinamic/tmc/ic/TMC4671/TMC4671.h"
#include "external/trinamic/tmc/ic/TMC4671/TMC4671_Register.h"
#include "external/trinamic/tmc/ic/TMC4671/TMC4671_Variants.h"
#include "external/trinamic/tmc/ic/TMC6100/TMC6100.h"
}

extern "C"
{
    // We need a static pointer here, because trinamic externs the following two
    // SPI binding functions that we need to interface with their API.
    //
    // The motor service exclusively calls the trinamic API which triggers these
    // functions. The motor service will set this variable in the constructor.
    static TmcMotorController* g_motor = NULL;

    uint8_t tmc4671_readwriteByte(uint8_t motor, uint8_t data, uint8_t last_transfer)
    {
        return g_motor->tmc4671ReadWriteByte(motor, data, last_transfer);
    }

    uint8_t tmc6100_readwriteByte(uint8_t motor, uint8_t data, uint8_t last_transfer)
    {
        return g_motor->tmc6100ReadWriteByte(motor, data, last_transfer);
    }
}

TmcMotorController::TmcMotorController()
    : spi_demux_select_0_(setupGpio(SPI_CS_DRIVER_TO_CONTROLLER_MUX_0_GPIO,
                GpioDirection::OUTPUT, GpioState::LOW)),
      spi_demux_select_1_(setupGpio(SPI_CS_DRIVER_TO_CONTROLLER_MUX_1_GPIO,
                                    GpioDirection::OUTPUT, GpioState::LOW)),
      driver_control_enable_gpio_(
          setupGpio(DRIVER_CONTROL_ENABLE_GPIO, GpioDirection::OUTPUT, GpioState::HIGH)),
      reset_gpio_(
          setupGpio(MOTOR_DRIVER_RESET_GPIO, GpioDirection::OUTPUT, GpioState::HIGH))
{
    openSpiFileDescriptor(front_left, FRONT_LEFT_MOTOR_CHIP_SELECT)
    openSpiFileDescriptor(front_right, FRONT_RIGHT_MOTOR_CHIP_SELECT)
    openSpiFileDescriptor(back_left, BACK_LEFT_MOTOR_CHIP_SELECT)
    openSpiFileDescriptor(back_right, BACK_RIGHT_MOTOR_CHIP_SELECT)
    openSpiFileDescriptor(dribbler, DRIBBLER_MOTOR_CHIP_SELECT)
}

void TmcMotorController::setup()
{   
    reset_gpio_->setValue(GpioState::LOW);
    usleep(MICROSECONDS_PER_MILLISECOND * 100);

    reset_gpio_->setValue(GpioState::HIGH);
    usleep(MICROSECONDS_PER_MILLISECOND * 100);

    for (uint8_t motor = 0; motor < NUM_MOTORS; ++motor)
    {
        LOG(INFO) << "Clearing RESET for " << MOTOR_NAMES[motor];
        tmc6100_writeInt(motor, TMC6100_GSTAT, 0x00000001);
        cached_motor_faults_[motor] = MotorFaultIndicator();
        encoder_calibrated_[motor]  = false;
    }

    // Drive Motor Setup
    for (uint8_t motor = 0; motor < NUM_DRIVE_MOTORS; motor++)
    {
        setUpDriveMotor(motor);
    }

    // Dribbler Motor Setup
    startDriver(DRIBBLER_MOTOR_CHIP_SELECT);
    checkDriverFault(DRIBBLER_MOTOR_CHIP_SELECT);
    startController(DRIBBLER_MOTOR_CHIP_SELECT, true);
    tmc4671_setTargetVelocity(DRIBBLER_MOTOR_CHIP_SELECT, 0);
    checkEncoderConnections();

    // calibrate the encoders
    for (uint8_t motor = 0; motor < NUM_DRIVE_MOTORS; motor++)
    {
        startEncoderCalibration(motor);
    }

    sleep(1);

    for (uint8_t motor = 0; motor < NUM_DRIVE_MOTORS; motor++)
    {
        endEncoderCalibration(motor);
    }
}

Motor::MotorFaultIndicator TmcMotorController::checkDriverFault(uint8_t motor)
{
    bool drive_enabled = true;
    std::unordered_set<TbotsProto::MotorFault> motor_faults;

    int gstat = tmc6100_readInt(motor, TMC6100_GSTAT);
    std::bitset<32> gstat_bitset(gstat);

    if (gstat_bitset.any())
    {
        LOG(WARNING) << "======= Faults For Motor " << std::to_string(motor) << "=======";
    }

    if (gstat_bitset[0])
    {
        LOG(WARNING)
            << "Indicates that the IC has been reset. All registers have been cleared to reset values."
            << "Attention: DRV_EN must be high to allow clearing reset";
        motor_faults.insert(TbotsProto::MotorFault::RESET);
    }

    if (gstat_bitset[1])
    {
        LOG(WARNING)
            << "drv_otpw : Indicates, that the driver temperature has exceeded overtemperature prewarning-level."
            << "No action is taken. This flag is latched.";
        motor_faults.insert(TbotsProto::MotorFault::DRIVER_OVERTEMPERATURE_PREWARNING);
    }

    if (gstat_bitset[2])
    {
        LOG(WARNING)
            << "drv_ot: Indicates, that the driver has been shut down due to overtemperature."
            << "This flag can only be cleared when the temperature is below the limit again."
            << "It is latched for information.";
        motor_faults.insert(TbotsProto::MotorFault::DRIVER_OVERTEMPERATURE);
    }

    if (gstat_bitset[3])
    {
        LOG(WARNING) << "uv_cp: Indicates an undervoltage on the charge pump."
                     << "The driver is disabled during undervoltage."
                     << "This flag is latched for information.";
        motor_faults.insert(TbotsProto::MotorFault::UNDERVOLTAGE_CHARGEPUMP);
        drive_enabled = false;
    }

    if (gstat_bitset[4])
    {
        LOG(WARNING) << "shortdet_u: Short to GND detected on phase U."
                     << "The driver becomes disabled until flag becomes cleared.";
        motor_faults.insert(TbotsProto::MotorFault::PHASE_U_SHORT_COUNTER_DETECTED);
        drive_enabled = false;
    }

    if (gstat_bitset[5])
    {
        LOG(WARNING) << "s2gu: Short to GND detected on phase U."
                     << "The driver becomes disabled until flag becomes cleared.";
        motor_faults.insert(TbotsProto::MotorFault::PHASE_U_SHORT_TO_GND_DETECTED);
        drive_enabled = false;
    }

    if (gstat_bitset[6])
    {
        LOG(WARNING) << "s2vsu: Short to VS detected on phase U."
                     << "The driver becomes disabled until flag becomes cleared.";
        motor_faults.insert(TbotsProto::MotorFault::PHASE_U_SHORT_TO_VS_DETECTED);
        drive_enabled = false;
    }

    if (gstat_bitset[8])
    {
        LOG(WARNING) << "shortdet_v: V short counter has triggered at least once.";
        motor_faults.insert(TbotsProto::MotorFault::PHASE_V_SHORT_COUNTER_DETECTED);
    }

    if (gstat_bitset[9])
    {
        LOG(WARNING) << "s2gv: Short to GND detected on phase V."
                     << "The driver becomes disabled until flag becomes cleared.";
        motor_faults.insert(TbotsProto::MotorFault::PHASE_V_SHORT_TO_GND_DETECTED);
        drive_enabled = false;
    }

    if (gstat_bitset[10])
    {
        LOG(WARNING) << "s2vsv: Short to VS detected on phase V."
                     << "The driver becomes disabled until flag becomes cleared.";
        motor_faults.insert(TbotsProto::MotorFault::PHASE_V_SHORT_TO_VS_DETECTED);
        drive_enabled = false;
    }

    if (gstat_bitset[12])
    {
        LOG(WARNING) << "shortdet_w: short counter has triggered at least once.";
        motor_faults.insert(TbotsProto::MotorFault::PHASE_W_SHORT_COUNTER_DETECTED);
    }

    if (gstat_bitset[13])
    {
        LOG(WARNING) << "s2gw: Short to GND detected on phase W."
                     << "The driver becomes disabled until flag becomes cleared.";
        motor_faults.insert(TbotsProto::MotorFault::PHASE_W_SHORT_TO_GND_DETECTED);
        drive_enabled = false;
    }

    if (gstat_bitset[14])
    {
        LOG(WARNING) << "s2vsw: Short to VS detected on phase W."
                     << "The driver becomes disabled until flag becomes cleared.";
        motor_faults.insert(TbotsProto::MotorFault::PHASE_W_SHORT_TO_VS_DETECTED);
        drive_enabled = false;
    }

    return MotorFaultIndicator(drive_enabled, motor_faults);
}

double TmcMotorController::readThenWriteValue(const MotorIndex motor, const int value)
{
    spi_demux_select_0_->setValue(GpioState::HIGH);
    spi_demux_select_1_->setValue(GpioState::LOW);
    // ensure tx_ and rx_ are cleared
    memset(read_tx_, 0, 5);
    memset(write_tx_, 0, 5);
    memset(read_rx_, 0, 5);

    //  Trinamic transactions looks like this:
    //  + - - - + - - - + - - - + - - - + - - - +
    //  |  ADDR |             DATA              |
    //  + - - - + - - - + - - - + - - - + - - - +
    //      0        1      2       3       4
    //  Also it is in BIG Endian, therefore MSB is leftmost bit of 0.
    //  For a write, MSB must be 1, for read, MSB must be 0
    //  https://github.com/trinamic/TMC-API/blob/master/tmc/ic/TMC4671/TMC4671.c
    read_tx_[0]  = read_addr & 0x7f;
    write_tx_[0] = write_addr | 0x80;

    // Convert from little endian to big endian
    for (int i = 3; i >= 0; i--)
    {
        uint8_t byte_to_copy = (uint8_t)(0xff & (write_data >> 8 * i));
        write_tx_[4 - i]     = byte_to_copy;
    }

    readThenWriteSpiTransfer(file_descriptors_[motor], read_tx_, write_tx_, read_rx_,
                             TMC4671_SPI_SPEED);

    int32_t value = read_rx_[0];
    for (int i = 1; i < 5; i++)
    {
        value <<= 8;
        value |= read_rx_[i];
    }
    return value;
}

void TmcMotorController::openSpiFileDescriptor(const MotorIndex& motor_index)
{
    file_descriptors_[motor_index] = open(SPI_PATHS[motor_index], O_RDWR);
    CHECK(file_descriptors_[motor_index] >= 0) << "can't open device: " << std::string(motor_index)
                                               << "error: " << strerror(errno);

    int ret = ioctl(file_descriptors_[motor_index], SPI_IOC_WR_MODE32, &SPI_MODE);
    CHECK(ret != -1) << "can't set spi mode for: " << std::string(motor_index)
                     << "error: " << strerror(errno);

    ret = ioctl(file_descriptors_[motor_index], SPI_IOC_WR_BITS_PER_WORD, &SPI_BITS);
    CHECK(ret != -1) << "can't set bits_per_word for: " << std::string(motor_index)
                     << "error: " << strerror(errno);

    ret = ioctl(file_descriptors_[motor_index], SPI_IOC_WR_MAX_SPEED_HZ, &MAX_SPI_SPEED_HZ);
    CHECK(ret != -1) << "can't set spi max speed hz for: " << std::string(motor_index)
                     << "error: " << strerror(errno);
}

void TmcMotorController::setUpDriveMotor(uint8_t motor)
{
    startDriver(motor);
    checkDriverFault(motor);
    // Start all the controllers as drive motor controllers
    startController(motor, false);
    tmc4671_setTargetVelocity(motor, 0);
}

uint8_t TmcMotorController::tmc4671ReadWriteByte(uint8_t motor, uint8_t data,
                                           uint8_t last_transfer)
{
    spi_demux_select_0_->setValue(GpioState::HIGH);
    spi_demux_select_1_->setValue(GpioState::LOW);
    return readWriteByte(motor, data, last_transfer, TMC4671_SPI_SPEED);
}

uint8_t TmcMotorController::tmc6100ReadWriteByte(uint8_t motor, uint8_t data,
                                           uint8_t last_transfer)
{
    spi_demux_select_0_->setValue(GpioState::LOW);
    spi_demux_select_1_->setValue(GpioState::HIGH);
    return readWriteByte(motor, data, last_transfer, TMC6100_SPI_SPEED);
}

uint8_t TmcMotorController::readWriteByte(uint8_t motor, uint8_t data, uint8_t last_transfer,
                                    uint32_t spi_speed)
{
    uint8_t ret_byte = 0;

    if (!transfer_started_)
    {
        memset(tx_, 0, sizeof(tx_));
        memset(rx_, 0, sizeof(rx_));
        position_ = 0;

        if (data & TMC_WRITE_BIT)
        {
            // If the transfer started and its a write operation,
            // set the appropriate flags.
            currently_reading_ = false;
            currently_writing_ = true;
        }
        else
        {
            // The first byte should contain the address on a read operation.
            // Trigger a transfer (1 byte) and buffer the response (4 bytes)
            tx_[position_] = data;
            spiTransfer(file_descriptors_[motor], tx_, rx_, 5, spi_speed);

            currently_reading_ = true;
            currently_writing_ = false;
        }

        transfer_started_ = true;
    }

    if (currently_writing_)
    {
        // Buffer the data to send out when last_transfer is true.
        tx_[position_++] = data;
    }

    if (currently_reading_)
    {
        // If we are reading, we just need to return the buffered data
        // byte by byte.
        ret_byte = rx_[position_++];
    }

    if (currently_writing_ && last_transfer)
    {
        // we have all the bytes for this transfer, lets trigger the transfer and
        // reset state
        spiTransfer(file_descriptors_[motor], tx_, rx_, 5, spi_speed);
        transfer_started_ = false;
    }

    if (currently_reading_ && last_transfer)
    {
        // when reading, if last transfer is true, we just need to reset state
        transfer_started_ = false;
    }

    return ret_byte;
}

void TmcMotorController::writeToDriverOrDieTrying(uint8_t motor, uint8_t address, int32_t value)
{
    int num_retires_left = NUM_RETRIES_SPI;
    int read_value       = 0;

    // The SPI lines have a lot of noise, and sometimes a transfer will fail
    // randomly. So we retry a few times before giving up.
    while (num_retires_left > 0)
    {
        tmc6100_writeInt(motor, address, value);
        read_value = tmc6100_readInt(motor, address);
        if (read_value == value)
        {
            return;
        }
        LOG(DEBUG) << "SPI Transfer to Driver Failed, retrying...";
        num_retires_left--;
    }

    // If we get here, we have failed to write to the driver. We reset
    // the chip to clear any bad values we just wrote and crash so everything stops.
    reset_gpio_->setValue(GpioState::LOW);
    CHECK(read_value == value) << "Couldn't write " << value
                               << " to the TMC6100 at address " << address
                               << " at address " << static_cast<uint32_t>(address)
                               << " on motor " << static_cast<uint32_t>(motor)
                               << " received: " << read_value;
}

void TmcMotorController::resetMotor()
{
    reset_gpio_->setValue(GpioState::LOW);
}
