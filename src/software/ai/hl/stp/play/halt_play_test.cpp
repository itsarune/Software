#include "software/ai/hl/stp/play/halt_play.h"

#include <gtest/gtest.h>

#include "software/simulated_tests/simulated_play_test_fixture.h"
#include "software/simulated_tests/terminating_validation_functions/robot_halt_validation.h"
#include "software/simulated_tests/validation/validation_function.h"
#include "software/test_util/test_util.h"
#include "software/time/duration.h"
#include "software/world/game_state.h"
#include "software/world/world.h"

class HaltPlayTest : public SimulatedPlayTestFixture
{
   protected:
    Field field = Field::createSSLDivisionBField();
};

TEST_F(HaltPlayTest, test_halt_play)
{
    BallState ball_state(Point(0, 0.5), Vector(0, 0));
    auto friendly_robots = TestUtil::createMovingRobotStatesWithId(
        {Point(-3, 2.5), Point(-3, 1.5), Point(-3, 0.5), Point(-3, -0.5), Point(-3, -1.5),
         Point(4.6, -3.1)},
        {Vector(1, 1), Vector(2, 1), Vector(1, 2), Vector(2, 2), Vector(1, 1),
         Vector(2, 1)});
    setFriendlyGoalie(0);
    auto enemy_robots = TestUtil::createStationaryRobotStatesWithId(
        {Point(1, 0), Point(1, 2.5), Point(1, -2.5), field.enemyGoalCenter(),
         field.enemyDefenseArea().negXNegYCorner(),
         field.enemyDefenseArea().negXPosYCorner()});
    setEnemyGoalie(0);
    setAIPlay(TYPENAME(HaltPlay));
    setRefereeCommand(RefereeCommand::HALT, RefereeCommand::HALT);

    std::vector<ValidationFunction> terminating_validation_functions = {

        [](std::shared_ptr<World> world_ptr, ValidationCoroutine::push_type& yield) {
            for (unsigned i = 0; i < 1000; i++)
            {
                robotHalt(world_ptr, yield);
            }
        }};

    std::vector<ValidationFunction> non_terminating_validation_functions = {};

    runTest(field, ball_state, friendly_robots, enemy_robots,
            terminating_validation_functions, non_terminating_validation_functions,
            Duration::fromSeconds(10));
}
