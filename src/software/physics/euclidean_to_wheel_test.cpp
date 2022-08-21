#include "software/physics/euclidean_to_wheel.h"

#include <gtest/gtest.h>

#include "software/test_util/test_util.h"

class EuclideanToWheelTest : public ::testing::Test
{
   protected:
    EuclideanToWheelTest() = default;
    EuclideanSpace_t target_euclidean_velocity{};
    WheelSpace_t expected_wheel_speeds{};
    WheelSpace_t calculated_wheel_speeds{};

    EuclideanToWheel euclidean_to_four_wheel =
        EuclideanToWheel(create2021RobotConstants());
};

TEST_F(EuclideanToWheelTest, test_target_wheel_speeds_zero)
{
    target_euclidean_velocity = {0, 0, 0};
    expected_wheel_speeds     = {0, 0, 0, 0};

    EXPECT_TRUE(TestUtil::equalWithinTolerance(
        expected_wheel_speeds,
        euclidean_to_four_wheel.getWheelVelocity(target_euclidean_velocity), 0.001));
}

// Note: The tests below assume that counter-clockwise motor rotation is positive velocity, and vise-versa.
TEST_F(EuclideanToWheelTest, test_target_wheel_speeds_positive_x)
{
    // Test +x/right
    target_euclidean_velocity = {0, 1, 0};
    calculated_wheel_speeds     = euclidean_to_four_wheel.getWheelVelocity(target_euclidean_velocity);
    std::cout << "calculated_wheel_speeds: " << calculated_wheel_speeds << std::endl;
    auto max_coeff = calculated_wheel_speeds.cwiseAbs().maxCoeff();

    auto max_vel = (calculated_wheel_speeds / max_coeff) *
        create2021RobotConstants().robot_max_speed_m_per_s;
    std::cout << "max_vel: " << max_vel << std::endl;
    std::cout << "euclidean_vel: " << euclidean_to_four_wheel.getEuclideanVelocity(max_vel) << std::endl;

    // Front wheels must be + velocity, back wheels must be - velocity.
    EXPECT_LT(calculated_wheel_speeds[0], 0);
    EXPECT_LT(calculated_wheel_speeds[1], 0);
    EXPECT_GT(calculated_wheel_speeds[2], 0);
    EXPECT_GT(calculated_wheel_speeds[3], 0);

    // max_speed = 4
    // +x 5.55691
    // +y 4.71929
}

TEST_F(EuclideanToWheelTest, test_target_wheel_speeds_negative_x)
{
    // Test -x/left
    target_euclidean_velocity = {-1, 0, 0};
    calculated_wheel_speeds     = euclidean_to_four_wheel.getWheelVelocity(target_euclidean_velocity);

    // Front wheels must be + velocity, back wheels must be - velocity.
    EXPECT_GT(calculated_wheel_speeds[0], 0);
    EXPECT_GT(calculated_wheel_speeds[1], 0);
    EXPECT_LT(calculated_wheel_speeds[2], 0);
    EXPECT_LT(calculated_wheel_speeds[3], 0);
}

TEST_F(EuclideanToWheelTest, test_target_wheel_speeds_positive_y)
{
    // Test +y/forwards
    target_euclidean_velocity = {0, 1, 0};
    calculated_wheel_speeds     = euclidean_to_four_wheel.getWheelVelocity(target_euclidean_velocity);

    // Right wheels must be + velocity, Left wheels must be - velocity.
    EXPECT_GT(calculated_wheel_speeds[0], 0);
    EXPECT_LT(calculated_wheel_speeds[1], 0);
    EXPECT_LT(calculated_wheel_speeds[2], 0);
    EXPECT_GT(calculated_wheel_speeds[3], 0);

    // Right wheels must have same velocity magnitude as left wheels, but opposite sign.
    EXPECT_DOUBLE_EQ(calculated_wheel_speeds[0], -calculated_wheel_speeds[1]);
    EXPECT_DOUBLE_EQ(calculated_wheel_speeds[2], -calculated_wheel_speeds[3]);
}

TEST_F(EuclideanToWheelTest, test_target_wheel_speeds_negative_y)
{
    // Test -y/backwards
    target_euclidean_velocity = {0, -1, 0};
    calculated_wheel_speeds     = euclidean_to_four_wheel.getWheelVelocity(target_euclidean_velocity);

    // Right wheels must be + velocity, Left wheels must be - velocity.
    EXPECT_LT(calculated_wheel_speeds[0], 0);
    EXPECT_GT(calculated_wheel_speeds[1], 0);
    EXPECT_GT(calculated_wheel_speeds[2], 0);
    EXPECT_LT(calculated_wheel_speeds[3], 0);

    // Right wheels must have same velocity magnitude as left wheels, but opposite sign.
    EXPECT_DOUBLE_EQ(calculated_wheel_speeds[0], -calculated_wheel_speeds[1]);
    EXPECT_DOUBLE_EQ(calculated_wheel_speeds[2], -calculated_wheel_speeds[3]);
}

TEST_F(EuclideanToWheelTest, test_target_wheel_speeds_positive_w)
{
    // Test +w/counter-clockwise
    target_euclidean_velocity = {0, 0, 1};
    calculated_wheel_speeds     = euclidean_to_four_wheel.getWheelVelocity(target_euclidean_velocity);

    // All wheels must be + velocity.
    EXPECT_GT(calculated_wheel_speeds[0], 0);
    EXPECT_GT(calculated_wheel_speeds[1], 0);
    EXPECT_GT(calculated_wheel_speeds[2], 0);
    EXPECT_GT(calculated_wheel_speeds[3], 0);

    // All wheels must have same velocity.
    EXPECT_DOUBLE_EQ(calculated_wheel_speeds[0], calculated_wheel_speeds[1]);
    EXPECT_DOUBLE_EQ(calculated_wheel_speeds[1], calculated_wheel_speeds[2]);
    EXPECT_DOUBLE_EQ(calculated_wheel_speeds[2], calculated_wheel_speeds[3]);
}

TEST_F(EuclideanToWheelTest, test_target_wheel_speeds_negative_w)
{
    // Test -w/clockwise
    target_euclidean_velocity = {0, 0, -1};
    calculated_wheel_speeds     = euclidean_to_four_wheel.getWheelVelocity(target_euclidean_velocity);

    // All wheels must be + velocity.
    EXPECT_LT(calculated_wheel_speeds[0], 0);
    EXPECT_LT(calculated_wheel_speeds[1], 0);
    EXPECT_LT(calculated_wheel_speeds[2], 0);
    EXPECT_LT(calculated_wheel_speeds[3], 0);

    // All wheels must have same velocity.
    EXPECT_DOUBLE_EQ(calculated_wheel_speeds[0], calculated_wheel_speeds[1]);
    EXPECT_DOUBLE_EQ(calculated_wheel_speeds[1], calculated_wheel_speeds[2]);
    EXPECT_DOUBLE_EQ(calculated_wheel_speeds[2], calculated_wheel_speeds[3]);
}

TEST_F(EuclideanToWheelTest, test_conversion_is_linear)
{
    target_euclidean_velocity = {3, 1, 5};
    auto result = euclidean_to_four_wheel.getWheelVelocity(target_euclidean_velocity);

    target_euclidean_velocity = {300, 100, 500};
    auto scaled_result =
        euclidean_to_four_wheel.getWheelVelocity(target_euclidean_velocity);

    EXPECT_TRUE(TestUtil::equalWithinTolerance(result * 100, scaled_result, 0.001));
}

TEST_F(EuclideanToWheelTest, test_double_convertion)
{
    // Converting from euclidean to wheel velocity and back should result in the same value
    target_euclidean_velocity = {3, 1, 5};

    auto wheel_velocity = euclidean_to_four_wheel.getWheelVelocity(target_euclidean_velocity);
    auto calculated_euclidean_velocity =
            euclidean_to_four_wheel.getEuclideanVelocity(wheel_velocity);

    EXPECT_TRUE(TestUtil::equalWithinTolerance(target_euclidean_velocity, calculated_euclidean_velocity, 0.001));
}
