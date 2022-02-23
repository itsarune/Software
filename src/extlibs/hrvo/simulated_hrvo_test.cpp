#include <gtest/gtest.h>

#include <utility>

#include "software/ai/hl/stp/tactic/move/move_tactic.h"
#include "software/simulated_tests/simulated_er_force_sim_tactic_test_fixture.h"
#include "software/simulated_tests/validation/validation_function.h"
#include "software/test_util/test_util.h"
#include "software/time/duration.h"
#include "software/world/world.h"

class SimulatedHRVOTest : public SimulatedErForceSimTacticTestFixture
{
   protected:
    FieldType field_type = FieldType::DIV_B;
    Field field          = Field::createField(field_type);
};

TEST_F(SimulatedHRVOTest, test_three_robot_wall)
{
    Point destination      = Point(4, 0);
    Point initial_position = Point(0, 0);
    BallState ball_state(Point(1, 2), Vector(0, 0));
    auto friendly_robots =
        TestUtil::createStationaryRobotStatesWithId({Point(-3, 0), initial_position});
    auto enemy_robots = TestUtil::createStationaryRobotStatesWithId(
        {Point(1, 0), Point(1, 0.3), Point(1, -0.3)});

    auto tactic = std::make_shared<MoveTactic>();
    tactic->updateControlParams(destination, Angle::zero(), 0);
    setTactic(tactic);
    setFriendlyRobotId(1);

    std::vector<ValidationFunction> terminating_validation_functions     = {};
    std::vector<ValidationFunction> non_terminating_validation_functions = {};

    runTest(field_type, ball_state, friendly_robots, enemy_robots,
            terminating_validation_functions, non_terminating_validation_functions,
            Duration::fromSeconds(15));
}

TEST_F(SimulatedHRVOTest, test_start_in_local_minimia)
{
    Point destination      = Point(4, 0);
    Point initial_position = Point(0.7, 0);
    BallState ball_state(Point(1, 2), Vector(0, 0));
    auto friendly_robots =
        TestUtil::createStationaryRobotStatesWithId({Point(-3, 0), initial_position});
    auto enemy_robots = TestUtil::createStationaryRobotStatesWithId(
        {Point(1, 0), Point(1, 0.3), Point(1, 0.6), Point(0.7, 0.6), Point(1, -0.3), Point(1, -0.6), Point(0.7, -0.6)});

    auto tactic = std::make_shared<MoveTactic>();
    tactic->updateControlParams(destination, Angle::zero(), 0);
    setTactic(tactic);
    setFriendlyRobotId(1);

    std::vector<ValidationFunction> terminating_validation_functions     = {};
    std::vector<ValidationFunction> non_terminating_validation_functions = {};

    runTest(field_type, ball_state, friendly_robots, enemy_robots,
            terminating_validation_functions, non_terminating_validation_functions,
            Duration::fromSeconds(15));
}

TEST_F(SimulatedHRVOTest, test_start_in_local_minimia_with_open_end)
{
    Point destination      = Point(4, 0);
    Point initial_position = Point(0.7, 0);
    BallState ball_state(Point(1, 2), Vector(0, 0));
    auto friendly_robots =
        TestUtil::createStationaryRobotStatesWithId({Point(-3, 0), initial_position});
    auto enemy_robots = TestUtil::createStationaryRobotStatesWithId(
        {Point(2, 0), Point(1, 0.3), Point(1, 0.6), Point(0.7, 0.6), Point(1, -0.3), Point(1, -0.6), Point(0.7, -0.6)});

    auto tactic = std::make_shared<MoveTactic>();
    tactic->updateControlParams(destination, Angle::zero(), 0);
    setTactic(tactic);
    setFriendlyRobotId(1);

    std::vector<ValidationFunction> terminating_validation_functions     = {};
    std::vector<ValidationFunction> non_terminating_validation_functions = {};

    runTest(field_type, ball_state, friendly_robots, enemy_robots,
            terminating_validation_functions, non_terminating_validation_functions,
            Duration::fromSeconds(15));
}

TEST_F(SimulatedHRVOTest, test_single_enemy_directly_infront)
{
    Point destination      = Point(2, 0);
    Point initial_position = Point(0.7, 0);
    BallState ball_state(Point(1, 2), Vector(0, 0));
    auto friendly_robots =
        TestUtil::createStationaryRobotStatesWithId({Point(-3, 0), initial_position});
    auto enemy_robots = TestUtil::createStationaryRobotStatesWithId(
        {Point(1, 0)});

    auto tactic = std::make_shared<MoveTactic>();
    tactic->updateControlParams(destination, Angle::zero(), 0);
    setTactic(tactic);
    setFriendlyRobotId(1);

    std::vector<ValidationFunction> terminating_validation_functions     = {};
    std::vector<ValidationFunction> non_terminating_validation_functions = {};

    runTest(field_type, ball_state, friendly_robots, enemy_robots,
            terminating_validation_functions, non_terminating_validation_functions,
            Duration::fromSeconds(15));
}

TEST_F(SimulatedHRVOTest, test_zig_zag_movement)
{
    // The x value of the wall in front of the friendly robot
    int front_wall_x = -1;
    // each gate refers to the center to center distance between each wall and the front
    // wall The constant offsets can be tweaked to get different distances between each
    // wall
    int gate_1           = 1;
    int gate_2           = gate_1 + 2;
    int gate_3           = gate_2 + 1;
    double robot_y_delta = 0.2;

    Point destination      = Point(front_wall_x + gate_3 + 0.5, 0);
    Point initial_position = Point(front_wall_x - 0.5, 0);
    BallState ball_state(Point(0, -2), Vector(0, 0));
    auto friendly_robots =
        TestUtil::createStationaryRobotStatesWithId({Point(-3, 0), initial_position});
    auto enemy_robots = TestUtil::createStationaryRobotStatesWithId(
        {Point(front_wall_x, 0.0), Point(front_wall_x, robot_y_delta),
         Point(front_wall_x, 2 * robot_y_delta), Point(front_wall_x + gate_1, 0.0),
         Point(front_wall_x + gate_1, -robot_y_delta),
         Point(front_wall_x + gate_1, -2 * robot_y_delta),
         Point(front_wall_x + gate_2, 0.0), Point(front_wall_x + gate_2, robot_y_delta),
         Point(front_wall_x + gate_2, 2 * robot_y_delta),
         Point(front_wall_x + gate_3, 0.0), Point(front_wall_x + gate_3, -robot_y_delta),
         Point(front_wall_x + gate_3, -2 * robot_y_delta)});

    auto tactic = std::make_shared<MoveTactic>();
    tactic->updateControlParams(destination, Angle::zero(), 0);
    setTactic(tactic);
    setFriendlyRobotId(1);

    std::vector<ValidationFunction> terminating_validation_functions     = {};
    std::vector<ValidationFunction> non_terminating_validation_functions = {};

    runTest(field_type, ball_state, friendly_robots, enemy_robots,
            terminating_validation_functions, non_terminating_validation_functions,
            Duration::fromSeconds(23));
}
