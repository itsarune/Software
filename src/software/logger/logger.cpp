#include "software/logger/logger.h"

LoggerSingleton::attachNetworkSink(int channel, const std::string& interface, int robot_id)
{
    auto network_log_sink_handle = logWorker->addSink(
        std::make_unique<struct NetworkSink>(channel, interface, robot_id),
        &NetworkSink::sendToNetwork);
}
