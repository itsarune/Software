#include "software/simulated_tests/simulated_er_force_sim_play_test_fixture.h"

#include "proto/message_translation/primitive_google_to_nanopb_converter.h"
#include "proto/message_translation/tbots_protobuf.h"
#include "software/test_util/test_util.h"

SimulatedErForceSimPlayTestFixture::SimulatedErForceSimPlayTestFixture()
    : ai_config(TbotsProto::AiConfig()),
      sensor_fusion_config(TbotsProto::SensorFusionConfig()),
      game_state(),
      ai(ai_config)
{
}

void SimulatedErForceSimPlayTestFixture::SetUp()
{
    SimulatedErForceSimTestFixture::SetUp();

    sensor_fusion_config = TbotsProto::SensorFusionConfig();
    ai                   = AI(ai_config);
}

void SimulatedErForceSimPlayTestFixture::setFriendlyGoalie(RobotId goalie_id)
{
    sensor_fusion_config.set_friendly_goalie_id(static_cast<int>(goalie_id));
}

void SimulatedErForceSimPlayTestFixture::setEnemyGoalie(RobotId goalie_id)
{
    sensor_fusion_config.set_enemy_goalie_id(static_cast<int>(goalie_id));
}

void SimulatedErForceSimPlayTestFixture::setAIPlay(const TbotsProto::Play& ai_play)
{
    ai_config.mutable_ai_control_config()->set_override_ai_play(ai_play);
}

void SimulatedErForceSimPlayTestFixture::setAIPlay(std::unique_ptr<Play> play)
{
    ai.overridePlay(std::move(play));
}

void SimulatedErForceSimPlayTestFixture::setRefereeCommand(
    const RefereeCommand& current_referee_command,
    const RefereeCommand& previous_referee_command)
{
    game_state.updateRefereeCommand(previous_referee_command);
    game_state.updateRefereeCommand(current_referee_command);
}

void SimulatedErForceSimPlayTestFixture::setGameState(const GameState& game_state_)
{
    game_state = game_state_;
}

void SimulatedErForceSimPlayTestFixture::updatePrimitives(
    const World& friendly_world, const World&,
    std::shared_ptr<ErForceSimulator> simulator_to_update)
{
    auto world_with_updated_game_state = friendly_world;
    world_with_updated_game_state.updateGameState(game_state);

    auto start_tick_time = std::chrono::system_clock::now();

    auto primitive_set_msg = ai.getPrimitives(world_with_updated_game_state);
    double duration_ms     = ::TestUtil::millisecondsSince(start_tick_time);
    registerFriendlyTickTime(duration_ms);
    auto world_msg = createWorld(world_with_updated_game_state);
    simulator_to_update->setYellowRobotPrimitiveSet(*primitive_set_msg,
                                                    std::move(world_msg));
}

const TbotsProto::AiConfig SimulatedErForceSimPlayTestFixture::getAiConfig() const
{
    return ai_config;
}

std::optional<TbotsProto::PlayInfo> SimulatedErForceSimPlayTestFixture::getPlayInfo()
{
    return ai.getPlayInfo();
}
