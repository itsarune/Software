#include "software/ai/ai.h"

#include <chrono>

#include "software/ai/hl/stp/play/halt_play.h"

AI::AI(std::shared_ptr<const AiConfig> ai_config)
    : ai_config(ai_config),
      fsm(std::make_unique<FSM<PlaySelectionFSM>>(PlaySelectionFSM{ai_config})),
      override_play(nullptr),
      current_play(std::make_unique<HaltPlay>(ai_config)),
      path_planner_factory(ai_config->getRobotNavigationObstacleConfig(),
                           // TODO: somehow do a look up??
                           Field::createSSLDivisionBField()),
      prev_override(false),
      prev_override_name("")
{
}

void AI::overridePlay(std::unique_ptr<Play> play)
{
    override_play = std::move(play);
}

void AI::overridePlayFromName(std::string name)
{
    overridePlay(GenericFactory<std::string, Play, AiConfig>::create(name, ai_config));
}

void AI::checkAiConfig()
{
    bool current_override = ai_config->getAiControlConfig()->getOverrideAiPlay()->value();
    std::string current_override_name =
        ai_config->getAiControlConfig()->getCurrentAiPlay()->value();
    if (current_override != prev_override || current_override_name != prev_override_name)
    {
        overridePlayFromName(
            ai_config->getAiControlConfig()->getCurrentAiPlay()->value());
    }
    prev_override      = current_override;
    prev_override_name = current_override_name;
}

std::unique_ptr<TbotsProto::PrimitiveSet> AI::getPrimitives(const World& world)
{
    checkAiConfig();
    fsm->process_event(PlaySelectionFSM::Update(
        [this](std::unique_ptr<Play> play) { current_play = std::move(play); },
        world.gameState()));

    if (static_cast<bool>(override_play))
    {
        return override_play->get(path_planner_factory, world, inter_play_communication,
                                  [this](InterPlayCommunication comm) {
                                      inter_play_communication = std::move(comm);
                                  });
    }
    else
    {
        return current_play->get(path_planner_factory, world, inter_play_communication,
                                 [this](InterPlayCommunication comm) {
                                     inter_play_communication = std::move(comm);
                                 });
    }

    TbotsProto::PlayInfo AI::getPlayInfo() const
    {
        std::string info_play_name   = objectTypeName(*current_play);
        auto robot_tactic_assignment = current_play->getRobotTacticAssignment();

        if (static_cast<bool>(override_play))
        {
            info_play_name          = objectTypeName(*override_play);
            robot_tactic_assignment = override_play->getRobotTacticAssignment();
        }

        TbotsProto::PlayInfo info;
        info.mutable_play()->set_play_name(info_play_name);

        for (const auto& [tactic, robot_id] : robot_tactic_assignment)
        {
            TbotsProto::PlayInfo_Tactic tactic_msg;
            tactic_msg.set_tactic_name(objectTypeName(*tactic));
            tactic_msg.set_tactic_fsm_state(tactic->getFSMState());
            (*info.mutable_robot_tactic_assignment())[robot_id] = tactic_msg;
        }

        return info;
    }
