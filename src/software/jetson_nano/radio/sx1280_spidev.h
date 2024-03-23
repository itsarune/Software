#pragma once

class Sx1280Spidev
{
    public:
        Sx1280Spidev();

    private:
        int fd_;

        // TODO(arun): add the actual pin numbers
        static constexpr int SPI_MOSI = 0;
        static constexpr int SPI_MISO = 0;
        static constexpr int SPI_SCK = 0;

        static constexpr int MAX_SPI_SPEED_HZ = 2000000;

        static constexpr int SPI_MODE_0 = 0;  // CPOL = 0, CPHA = 0
        static constexpr const char* SPI_PATH = "/dev/spidev1.0";
};
