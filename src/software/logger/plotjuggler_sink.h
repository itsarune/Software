#pragma once
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <fstream>
#include <g3log/logmessage.hpp>
#include <iostream>
#include <string>

#include "proto/visualization.pb.h"
#include "software/logger/custom_logging_levels.h"


/**
 * This class acts as a custom sink for g3log. In particular, it allows us to send
 * values to be plotted in PlotJuggler over UDP.
 */
class PlotJugglerSink
{
   public:
    /**
     * Creates a PlotJugglerSink that sends udp packets to the PlotJuggler server
     */
    PlotJugglerSink();

    virtual ~PlotJugglerSink();

    /**
     * This function is called on every call to LOG(). It sends a JSON string to
     * PlotJuggler over the network.
     *
     * @param log_entry the message received on a LOG() call
     */
    void sendToPlotJuggler(g3::LogMessageMover log_entry);

   private:
    // The io_service that will be used to service all network requests
    boost::asio::io_service io_service;

    // The thread running the io_service in the background. This thread will run for the
    // entire lifetime of the class
    std::thread io_service_thread;

    // A UDP socket to send data over
    boost::asio::ip::udp::socket socket_;

    // The endpoint for the receiver
    boost::asio::ip::udp::endpoint receiver_endpoint;

    // Buffer to hold serialized protobuf data
    std::string data_buffer;
};

/*
 * TODO
 *
 * @param os The output stream
 * @param message The message to serialize
 * @return The output stream containing
 */
std::ostream& operator<<(std::ostream& os,
                         const TbotsProto::PlotJugglerValue& plotjuggler_value);
