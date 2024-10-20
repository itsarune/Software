#include "software/ai/hl/stp/play/play_factory.h"

#include "proto/message_translation/tbots_geometry.h"
#include "proto/message_translation/tbots_protobuf.h"
#include "software/logger/logger.h"
#include "software/util/generic_factory/generic_factory.h"

std::unique_ptr<Play> createPlay(const TbotsProto::Play& play_proto,
                                 std::shared_ptr<const AiConfig> ai_config)
{
    return GenericFactory<std::string, Play, AiConfig>::create(
        TbotsProto::Play_Name(play_proto), ai_config);
}
