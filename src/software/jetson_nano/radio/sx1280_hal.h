#pragma once

#include "software/jetson_nano/radio/sx1280_spidev.h"

// Derived from Semtech's mbed library implementation: https://os.mbed.com/teams/Semtech/code/SX1280Lib/

class Sx1280Hal
{
    public:
        Sx1280Hal();

    // required functions
    // setSleep(RETAIN_None)
    // setMode(MODE_STDBY_XOSC)
    // setRegulatorMode(USE_DCDC)
    // setPacketType(PACKET_TYPE_FLRC)
    // setRfFrequency(0, 0);
    // setModulationParams(BandwidthBitRate, CodingRate, BT)
    // LT.setPacketParams(PREAMBLE_LENGTH_16_BITS, FLRC_SYNC_WORD_LEN_P32S, RADIO_RX_MATCH_SYNCWORD_1, RADIO_PACKET_VARIABLE_LENGTH, 32, RADIO_CRC_OFF, RADIO_WHITENING_OFF);
    // LT.setDioIrqParams(IRQ_RADIO_ALL, (IRQ_TX_DONE + IRQ_RX_TX_TIMEOUT), 0, 0);  //set for IRQ on TX done and timeout on DIO1
    // LT.setSyncWord1(Syncword);
    // LT.setSyncWordErrorTolerance(2);
    // LT.setAutoFS(0x01);
    // LT.setTxParams(TXpower, RADIO_RAMP_04_US);
    // LT.setFS();
    // LT.clearIrqStatus(IRQ_RADIO_ALL);

    private:
        Sx1280Spidev radio_;
};
