#pragma once

#include <cstdint>
#include <string.h>

    /**
     * Trigger an SPI transfer over an open SPI connection
     *
     * @param fd The SPI File Descriptor to transfer data over
     * @param tx The tx buffer, data to send out
     * @param rx The rx buffer, will be updated with data from the full-duplex transfer
     * @param len The length of the tx and rx buffer
     * @param spi_speed The speed to run spi at
     *
     */
    void spiTransfer(int fd, uint8_t const* tx, uint8_t const* rx, unsigned len,
                     uint32_t spi_speed);
