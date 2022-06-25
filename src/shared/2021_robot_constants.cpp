#include "shared/2021_robot_constants.h"

#include "shared/constants.h"

RobotConstants_t create2021RobotConstants(void)
{
    RobotConstants_t robot_constants = {
        .mass_kg         = 2.5f,   // determined experimentally
        .inertial_factor = 0.37f,  // determined experimentally
        .robot_radius_m  = ROBOT_MAX_RADIUS_METERS,
        // TODO (#2112): update this
        .jerk_limit_kg_m_per_s_3 = 40.0f,
        .front_wheel_angle_deg   = 50.0f,
        .back_wheel_angle_deg    = 45.0f,
        // TODO (#2112): update this
        .front_of_robot_width_meters = 0.11f,
        // TODO (#2112): update this
        .dribbler_width_meters                  = 0.088f,
        .robot_max_speed_m_per_s                = 4.000f,
        .robot_max_ang_speed_rad_per_s          = 56.76f,
        .robot_max_acceleration_m_per_s_2       = 2.0f,
        .robot_max_ang_acceleration_rad_per_s_2 = 38.57f,
        // TODO (#2112): update this
        .indefinite_dribbler_speed_rpm = 10000.0f,
        // TODO (#2112): update this
        .max_force_dribbler_speed_rpm       = 16000.0f,
        .wheel_radius_meters                = 0.03f,
        .wheel_rotations_per_motor_rotation = 17.0f / 60.0f};
    return robot_constants;
}
