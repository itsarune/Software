#pragma once

#include <functional>

#include "software/ai/hl/stp/stp.h"
#include "software/ai/navigator/navigator.h"
#include "software/time/timestamp.h"
#include "software/world/world.h"

/**
 * This class wraps all our AI logic and decision making.
 */
class AI final
{
   public:
    AI() = delete;

    /**
     * Create an AI with given configurations
     * @param ai_config_ The AI configuration
     */
    explicit AI(std::shared_ptr<const AiConfig> ai_config_);

    /**
     * Overrides the play
     *
     * @param play play to override with
     */
    void overridePlay(std::unique_ptr<Play> play);

    /**
     * Calculates the Primitives that should be run by our Robots given the current
     * state of the world.
     *
     * @param world The state of the World with which to make the decisions
     *
     * @return the Primitives that should be run by our Robots given the current
     * state of the world.
     */
    std::unique_ptr<TbotsProto::PrimitiveSet> getPrimitives(const World& world);

    /**
     * Returns information about the currently running plays and tactics, including the
     * name of the play, and which robots are running which tactics
     *
     * @return information about the currently running plays and tactics
     */
    TbotsProto::PlayInfo getPlayInfo() const;

    std::shared_ptr<Navigator> getNavigator() const;

   private:
    /**
     * Overrides the play from the name
     *
     * @param name the play name
     */
    void overridePlayFromName(std::string name);

    std::shared_ptr<const AiConfig> ai_config;
    std::shared_ptr<Navigator> navigator;
    std::unique_ptr<FSM<PlaySelectionFSM>> fsm;
    std::unique_ptr<Play> override_play;
    std::unique_ptr<Play> current_play;
    GlobalPathPlannerFactory path_planner_factory;
};
