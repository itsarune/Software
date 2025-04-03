#include "software/embedded/spi_utils.h"

#include "software/logger/logger.h"

#include <errno.h>
#include <linux/ioctl.h>
#include <linux/spi/spidev.h>

void spiTransfer(int fd, uint8_t const* tx, uint8_t const* rx, unsigned len,
                               uint32_t spi_speed)
{
    int ret;

    struct spi_ioc_transfer tr[1];
    memset(tr, 0, sizeof(tr));

    tr[0].tx_buf        = (unsigned long)tx;
    tr[0].rx_buf        = (unsigned long)rx;
    tr[0].len           = len;
    tr[0].delay_usecs   = 0;
    tr[0].speed_hz      = spi_speed;
    tr[0].bits_per_word = 8;

    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);

    CHECK(ret >= 1) << "SPI Transfer to motor failed, not safe to proceed: errno "
                    << strerror(errno);
}
