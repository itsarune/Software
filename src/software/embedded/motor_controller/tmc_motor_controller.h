#pragma once

#include "software/embedded/gpio/gpio.h"
#include "software/embedded/gpio/gpio_char_dev.h"
#include "software/embedded/gpio/gpio_sysfs.h"
#include "software/embedded/motor_controller/motor_controller.h"
#include "software/embedded/motor_controller/motor_fault_indicator.h"
#include "software/embedded/motor_controller/motor_index.h"

class TmcMotorController : public MotorController
{
    public:
    TmcMotorController();
        
    void reset() override;

    MotorFaultIndicator checkDriverFault(MotorIndex motor) override;

    double readThenWriteValue(const MotorIndex motor, const int value) override;

    private:
    /**
     * Helper function to setup a GPIO pin. Selects the appropriate GPIO implementation
     * based on the host platform.
     *
     * @tparam T The representation of the GPIO number
     * @param gpio_number The GPIO number (this is typically different from the hardware
     * pin number)
     * @param direction The direction of the GPIO pin (input or output)
     * @param initial_state The initial state of the GPIO pin (high or low)
     */
    template <typename T>
    static std::unique_ptr<Gpio> setupGpio(const T& gpio_number, GpioDirection direction,
                                           GpioState initial_state);

    /**
     * Opens SPI File Descriptor
     *
     * @param motor_index The index of the motor to open
     */
    void openSpiFileDescriptor(const std::string& motor_name);

    void setUpDriveMotor(uint8_t motor);

    // Both the TMC4671 (the controller) and the TMC6100 (the driver) respect
    // the same SPI interface. So when we bind the API, we can use the same
    // readWriteByte function, provided that the chip select pin is turning on
    // the right chip.
    //
    // Each TMC4671 controller, TMC6100 driver and encoder group have their chip
    // selects coming in from a demux (see diagram below). The demux is controlled
    // by two bits {spi_demux_select_0, spi_demux_select_1}. If the bits are
    // 10 the TMC4671 is selected, when the select bits are 01 the TMC6100 is
    // selected and when they are 11 the encoder is selected. 00 disconnects all
    // 3 chips.
    //
    //
    //                                      FRONT LEFT MOTOR
    //                                 CONTROLLER + DRIVER + ENCODER
    //
    //                    ┌───────┐        ┌───────────────┐
    //                    │       │        │               │
    //                    │  2:4  │  10    │  ┌─────────┐  │
    //                    │       ├────────┼──►TMC4671  │  │  B0
    //     FRONT_LEFT_CS  │ DEMUX │        │  └─────────┘  │
    //     ───────────────►       │        │               │
    //                    │       │  01    │  ┌─────────┐  │
    //                    │       ├────────┼──►TMC6100  │  │  B1
    //                    │       │        │  └─────────┘  │
    //                    │       │        │               │
    //                    │       │  11    │  ┌─────────┐  │
    //                    │       ├────────┼──►ENCODER  │  │  B2
    //                    │       │        │  └─────────┘  │
    //                    └───▲───┘        │               │
    //                        │            └───────────────┘
    //                        │
    //                spi_demux_sel_0 & 1
    //
    uint8_t tmc4671ReadWriteByte(uint8_t motor, uint8_t data,
                                           uint8_t last_transfer);

    uint8_t tmc6100ReadWriteByte(uint8_t motor, uint8_t data,
                                           uint8_t last_transfer);

    uint8_t readWriteByte(uint8_t motor, uint8_t data, uint8_t last_transfer,
                                    uint32_t spi_speed);

    void writeToDriverOrDieTrying(uint8_t motor, uint8_t address, int32_t value);

    void resetMotor();

    // Select between driver and controller gpio
    std::unique_ptr<Gpio> spi_demux_select_0_;
    std::unique_ptr<Gpio> spi_demux_select_1_;

    // Enable driver gpio
    std::unique_ptr<Gpio> driver_control_enable_gpio_;
    std::unique_ptr<Gpio> reset_gpio_;

    // Transfer Buffers for spiTransfer
    uint8_t tx_[5] = {0};
    uint8_t rx_[5] = {0};

    // Transfer Buffers for readThenWriteSpiTransfer
    uint8_t write_tx_[5] = {0};
    uint8_t read_tx_[5]  = {0};
    uint8_t read_rx_[5]  = {0};

    // Transfer State
    bool transfer_started_  = false;
    bool currently_writing_ = false;
    bool currently_reading_ = false;
    uint8_t position_       = 0;

    // SPI Trinamic Motor Driver Paths
    static const inline std::unordered_map<MotorIndex, std::string> SPI_PATHS = {
        {MotorIndex::FRONT_LEFT, "/dev/spidev0.0"},
        {MotorIndex::FRONT_RIGHT, "/dev/spidev0.1"},
        {MotorIndex::BACK_LEFT, "/dev/spidev0.2"},
        {MotorIndex::BACK_RIGHT, "/dev/spidev0.3"},
        {MotorIndex::DRIBBLER, "/dev/spidev0.4"},
    };

    // SPI Configs
    static constexpr uint32_t TMC6100_SPI_SPEED = 1000000;  // 1 Mhz
    static constexpr uint32_t TMC4671_SPI_SPEED = 1000000;  // 1 Mhz
    static constexpr uint32_t MAX_SPI_SPEED_HZ  = 2000000;  // 2 Mhz
    static constexpr uint8_t SPI_BITS           = 8;
    static constexpr uint32_t SPI_MODE          = 0x3u;

    // Motor names (indexed with chip select above)
    static constexpr const char* MOTOR_NAMES[] = {"front_left", "back_left", "back_right",
                                                  "front_right", "dribbler"};
};

template <typename T>
std::unique_ptr<Gpio> MotorService::setupGpio(const T& gpio_number,
                                              GpioDirection direction,
                                              GpioState initial_state)
{
    if constexpr (PLATFORM == Platform::JETSON_NANO)
    {
        return std::make_unique<GpioSysfs>(gpio_number, direction, initial_state);
    }
    else
    {
        return std::make_unique<GpioCharDev>(gpio_number, direction, initial_state,
                                             "/dev/gpiochip4");
    }
}
