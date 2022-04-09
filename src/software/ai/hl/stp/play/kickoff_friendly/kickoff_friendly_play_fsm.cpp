#include "software/ai/hl/stp/play/kickoff_friendly/kickoff_friendly_play_fsm.h"

void KickoffFriendlyPlayFSM::moveToSetupPositions(const Update& event)
{
    PriorityTacticVector ret_tactics = {{}};
    
    std::vector<std::shared_ptr<MoveTactic>> move_tactics = std::vector<std::shared_ptr<MoveTactic>>(event.common.num_tactics);
    
    std::vector<Point> kickoff_setup_positions = {
        // Robot 1
        Point(event.common.world.field().centerPoint() +
              Vector(-event.common.world.field().centerCircleRadius() + 0.1, 0)),
        // Robot 3
        Point(event.common.world.field().centerPoint() +
              Vector(-event.common.world.field().centerCircleRadius() - 4 * ROBOT_MAX_RADIUS_METERS,
                     -1.0 / 3.0 * event.common.world.field().yLength())),
        // Robot 4
        Point(event.common.world.field().centerPoint() +
              Vector(-event.common.world.field().centerCircleRadius() - 4 * ROBOT_MAX_RADIUS_METERS,
                     1.0 / 3.0 * event.common.world.field().yLength()))
    };
    
    std::generate(move_tactics.begin(), move_tactics.end(), [](){ return std::make_shared<MoveTactic>(); });
    
    for (unsigned int i = 0; i < event.common.num_tactics; ++i)
    {
        move_tactics[i]->updateControlParams(kickoff_setup_positions.at(i), Angle::zero(), 0,
                                            TbotsProto::MaxAllowedSpeedMode::PHYSICAL_LIMIT);
        ret_tactics[0].push_back(move_tactics[i]);
    }
    
    event.common.set_tactics(ret_tactics);
}
