#include "software/logger/logger.h"

void LoggerSingleton::attachNetworkSink(int channel, const std::string& interface, int robot_id)
{
    auto network_log_sink_handle = logWorker->addSink(
        std::make_unique<struct NetworkSink>(channel, interface, robot_id),
        &NetworkSink::sendToNetwork);
}

void LoggerSingleton::attachProtobufSink(const std::string& runtime_dir) {
    // Sink for visualization
    auto visualization_handle = logWorker->addSink(
        std::make_unique<ProtobufSink>(runtime_dir), &ProtobufSink::sendProtobuf);
}
