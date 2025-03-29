#pragma once

class TmcMotorController : public MotorController
{
    public:
    TmcMotorController();
        
    void setup();

    Motor::MotorFaultIndicator TmcMotor::checkDriverFault(uint8_t motor) override;

    double readThenWriteValue(const MotorIndex motor, const int value) override;

    private:
    /**
     * Opens SPI File Descriptor
     *
     * @param motor_name The name of the motor the spi path is connected to
     * @param chip_select Which chip select to use
     */
    void openSpiFileDescriptor(const std::string& motor_name, const uint8_t& chip_select);

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

    void writeToDriverOrDieTrying(uint8_t motor, uint8_t address, int32_t value)

    void resetMotor();

    static constexpr uint32_t MAX_SPI_SPEED_HZ  = 2000000;  // 2 Mhz
    static constexpr uint8_t SPI_BITS           = 8;
    static constexpr uint32_t SPI_MODE          = 0x3u;

    // SPI Trinamic Motor Driver Paths (indexed with chip select above)
    static constexpr const char* SPI_PATHS[] = {"/dev/spidev0.0", "/dev/spidev0.1",
                                                "/dev/spidev0.2", "/dev/spidev0.3",
                                                "/dev/spidev0.4"};

    // Motor names (indexed with chip select above)
    static constexpr const char* MOTOR_NAMES[] = {"front_left", "back_left", "back_right",
                                                  "front_right", "dribbler"};
};
