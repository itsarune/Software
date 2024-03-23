#include "software/jetson_nano/radio/sx1280_spidev.h"

#include "software/logger/logger.h"

#include <linux/ioctl.h>
#include <linux/spi/spidev.h>

Sx1280Spidev::Sx1280Spidev()
{
    // Open the SPI device
    fd_ = open(SPI_PATH, O_RDWR);
    CHECK(fd_ >= 0) << "Failed to open Radio device: " << SPI_PATH;

    int ret = ioctl(fd_, SPI_IOC_WR_MODE32, &SPI_MODE_0);
    CHECK(ret != -1) << "Failed to set SPI mode, error: " << strerror(errno);

    ret = ioctl(fd_, SPI_IOC_WR_BITS_PER_WORD, 8);
    CHECK(ret != -1) << "Failed to set SPI bits per word, error: " << strerror(errno);

    ret = ioctl(fd_, SPI_IOC_WR_MAX_SPEED_HZ, MAX_SPI_SPEED_HZ);
    CHECK(ret != -1) << "Failed to set SPI speed, error: " << strerror(errno);
}
