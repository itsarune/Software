#include "software/ai/hl/stp/tactic/chip/chip_tactic.h"

#include <algorithm>


ChipTactic::ChipTactic()
    : Tactic({RobotCapability::Chip, RobotCapability::Move}),
      fsm{GetBehindBallFSM()},
      fsm_map()
{
    for (RobotId id = 0; id < MAX_ROBOT_IDS; id++)
    {
        fsm_map[id] = std::make_unique<FSM<ChipFSM>>(GetBehindBallFSM());
    }
}

void ChipTactic::updateControlParams(const Point &chip_origin,
                                     const Angle &chip_direction,
                                     double chip_distance_meters)
{
    control_params.chip_origin          = chip_origin;
    control_params.chip_direction       = chip_direction;
    control_params.chip_distance_meters = chip_distance_meters;
}

void ChipTactic::updateControlParams(const Point &chip_origin, const Point &chip_target)
{
    updateControlParams(chip_origin, (chip_target - chip_origin).orientation(),
                        (chip_target - chip_origin).length());
}

double ChipTactic::calculateRobotCost(const Robot &robot, const World &world) const
{
    // the closer the robot is to a ball, the cheaper it is to perform the chip
    double cost = (robot.position() - world.ball().position()).length() /
                  world.field().totalXLength();

    return std::clamp<double>(cost, 0, 1);
}

void ChipTactic::accept(TacticVisitor &visitor) const
{
    visitor.visit(*this);
}

void ChipTactic::updatePrimitive(const TacticUpdate &tactic_update, bool reset_fsm)
{
    if (reset_fsm)
    {
        fsm_map[tactic_update.robot.id()] =
            std::make_unique<FSM<ChipFSM>>(GetBehindBallFSM());
    }
    fsm.process_event(ChipFSM::Update(control_params, tactic_update));
}
