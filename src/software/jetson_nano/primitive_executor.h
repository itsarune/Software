#pragma once
#include "proto/primitive.pb.h"
#include "software/geom/vector.h"
#include "software/world/world.h"
#include "proto/tbots_software_msgs.pb.h"
#include "extlibs/hrvo/simulator.h"

class PrimitiveExecutor
{
   public:
    explicit PrimitiveExecutor(double time_step);

    /**
     * Start running a primitive
     *
     * @param primitive_set_msg The primitive to start
     */
    void updatePrimitiveSet(const unsigned int robot_id, const TbotsProto::PrimitiveSet &primitive_set_msg);

    void updateWorld(const TbotsProto::World& world_msg);

    /**
     * Steps the current primitive and returns a direct control primitive with the
     * target wheel velocities
     *
     * @param robot_state The current robot_state to step the primitive on
     * @returns DirectPerWheelControl The per-wheel direct control primitive msg
     */
    std::unique_ptr<TbotsProto::DirectControlPrimitive>
    stepPrimitive(const unsigned int robot_id, const RobotState &robot_state);

   private:
    /*
     * Compute the next target linear velocity the robot should be at
     * assuming max acceleration.
     *
     * @param primitive The MovePrimitive to compute the linear velocity for
     * @param robot_state The RobotState of the robot we are planning the current
     * primitive for
     * @returns Vector The target linear velocity
     */
    Vector getTargetLinearVelocity(const unsigned int robot_id,
                                   const RobotState& robot_state);

    /*
     * Compute the next target angular velocity the robot should be at
     * assuming max acceleration.
     *
     * @param primitive The MovePrimitive to compute the angular velocity for
     * @param robot_state The RobotState of the robot we are planning the current
     * primitive for
     * @returns AngularVelocity The target angular velocity
     */
    AngularVelocity getTargetAngularVelocity(const TbotsProto::MovePrimitive& primitive,
                                             const RobotState& robot_state);

    /*
     * The AutoKickOrChip settings from the move primitive need to get copied over
     * to the direct control primitive.
     *
     * TODO (#2340) Remove
     *
     * @param src The move primitive to copy from
     * @param dest The direct primitive to copy to
     */
    void copyAutoChipOrKick(const TbotsProto::MovePrimitive& src,
                            TbotsProto::DirectControlPrimitive* dest);

    TbotsProto::Primitive current_primitive_;
    RobotConstants_t robot_constants_;
    Simulator hrvo_simulator;
};
