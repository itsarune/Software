#pragma once

#include "software/networking/proto_radio_listener.hpp"
#include "shared/constants.h"

#include <thread>
#include <unistd.h>

class ThreadedProtoRadioListener
{
public:
    ThreadedProtoRadioListener(uint8_t channel,
                               int spi_speed=1400000);

    ~ThreadedProtoRadioListener();

    template <class ReceiveProtoT>
    void registerListener(const uint8_t addr[RADIO_ADDR_LENGTH], std::function<void(ReceiveProtoT)> callback);
private:
    // The thread running the io_service in the background. This thread will run for the
    // entire lifetime of the class
    static const unsigned int POLL_INTERVAL_MS = 10;

    std::thread radio_listener_thread;
    ProtoRadioListener radio_listener;
};

ThreadedProtoRadioListener::ThreadedProtoRadioListener(uint8_t channel, int spi_speed)
    : radio_listener(channel, spi_speed)
{
    // start the thread to run the io_service in the background
    std::cout << "starting radio listener thread" << std::endl;
    radio_listener_thread = std::thread([this]() {
        for (;;) {
            radio_listener.receive();
            usleep(POLL_INTERVAL_MS * MICROSECONDS_PER_MILLISECOND);
        }
    });
}


template <class ReceiveProtoT>
void ThreadedProtoRadioListener::registerListener(const uint8_t addr[RADIO_ADDR_LENGTH], std::function<void(ReceiveProtoT)> callback)
{
    auto packet_data = ReceiveProtoT();
    std::string raw_data;
    radio_listener.registerListener(addr, 
            [&raw_data](std::string raw_byte_data)
            {
                raw_data = raw_byte_data;
            });

    if (!packet_data.ParseFromArray(raw_data.data(), static_cast<int>(raw_data.length())))
    {
        LOG(WARNING) << "Packet of received type " << ReceiveProtoT::descriptor()->full_name() << " was corrupted";
        return;
    }
    callback(packet_data);
}

ThreadedProtoRadioListener::~ThreadedProtoRadioListener()
{
    radio_listener_thread.join();
}

