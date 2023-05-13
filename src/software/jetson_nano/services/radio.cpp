#include "software/jetson_nano/services/radio.h"

RadioService::RadioService(uint8_t channel, uint8_t multicast_level, uint8_t address, unsigned short world_listener_port,
                           unsigned short primitive_listener_port, unsigned short robot_status_sender_port)
        :
{
    // TODO: construct listeners and packet trackers
    sender = std::make_unique<ProtoRadioSender<TbotsProto::RobotStatus>>(channel, multicast_level, address);
    world_listener = std::make_unique<ThreadedProtoRadioListener<TbotsProto::World>>(
            channel, multicast_level, address, boost::bind(&RadioService::worldCallback, this, _1)
            );
    primitive_set_listener = std::make_unique<ThreadedProtoRadioListener<TbotsProto::PrimitiveSet>>(
            channel, multicast_level, address, boost::bind(&RadioService::primitiveSetCallback, this, _1)
    );
}

void RadioService::primitiveSetCallback(TbotsProto::PrimitiveSet input)
{
    std::scoped_lock<std::mutex> lock(primitive_set_mutex);
    const uint64_t seq_num = input.sequence_number();

    primitive_set_tracker.send(seq_num);
    if (primitive_set_tracker.isLastValid())
    {
        primitive_set_msg = input;
    }

    float primitive_set_loss_rate = primitive_set_tracker.getLossRate();
    if (primitive_set_loss_rate > PROTO_LOSS_WARNING_THRESHOLD)
    {
        LOG(WARNING) << "Primitive set loss rate is " << primitive_set_loss_rate * 100
                     << "%";
    }
}

void RadioService::worldCallback(TbotsProto::World input)
{
    std::scoped_lock<std::mutex> lock(world_mutex);
    const uint64_t seq_num = input.sequence_number();

    world_tracker.send(seq_num);
    if (world_tracker.isLastValid())
    {
        world_msg = input;
    }

    float world_loss_rate = world_tracker.getLossRate();
    if (world_loss_rate > PROTO_LOSS_WARNING_THRESHOLD)
    {
        LOG(WARNING) << "World loss rate is " << world_loss_rate * 100 << "%";
    }
}