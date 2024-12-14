import software.python_bindings as tbots_cpp
from proto.import_all_protos import *

from software.simulated_tests.validation import (
    Validation,
    create_validation_geometry,
    create_validation_types,
)


class KickerDoubleTouch(Validation):
    """Checks if the robot kicking the ball touches the ball again before another robot touches it."""

    KICKOFF_DOUBLE_TOUCH_M = 0.05
    VALIDATION_RADIUS_M = 0.5

    def __init__(self, threshold=0.1):
        """
        :param threshold: The distance from the ball to consider a robot touching it
        """
        self.kicker_robot_id = None
        self.kick_position = None
        self.ball_position = None
        self.kick_completed = False
        self.threshold = threshold

    def get_validation_status(self, world) -> ValidationStatus:
        """Checks if the robot kicking the ball touches the ball again before another robot touches it

        :param world: The world msg to validate
        :returns: FAILING when the robot kicking the ball touches the ball again before another robot touches it
                  PASSING when the robot kicking the ball does not touch the ball again before another robot touches it
        """
        if self.kick_completed:
            return ValidationStatus.PASSING

        if len(world.friendly_team.team_robots) == 0:
            return ValidationStatus.PASSING

        self.ball_position = tbots_cpp.createPoint(world.ball.current_state.global_position)
        if self.kick_position is None:
            self.kick_position = self.ball_position

        # Assign the kicker robot if it is not already assigned
        kicker_robot = None
        if self.kicker_robot_id is None:
            kicker_robot = tbots_cpp.Robot(world.friendly_team.team_robots[0])
            for robot in world.friendly_team.team_robots:
                current_kicker_position = kicker_robot.position()
                robot_position = tbots_cpp.createPoint(robot.current_state.global_position)
                if (self.ball_position - robot_position).length() < (self.ball_position - current_kicker_position).length():
                    self.kicker_robot_id = robot.id
                    kicker_robot = tbots_cpp.Robot(robot)
        else:
            for robot in world.friendly_team.team_robots:
                if robot.id == self.kicker_robot_id:
                    kicker_robot = tbots_cpp.Robot(robot)

        # Check if another robot is closer to the ball than the kicker
        ball_to_kicker_dist = (self.ball_position - kicker_robot.position()).length()
        for robot in world.friendly_team.team_robots:
            robot_position = tbots_cpp.createPoint(robot.current_state.global_position)
            if robot.id != self.kicker_robot_id and (self.ball_position - robot_position).length() < ball_to_kicker_dist:
                self.kick_completed = True
                return ValidationStatus.PASSING

        # Check if we have exceeded the double touch distance
        if (self.ball_position - self.kick_position).length() > KickerDoubleTouch.KICKOFF_DOUBLE_TOUCH_M\
                and kicker_robot.isNearDribbler(self.ball_position):
            return ValidationStatus.FAILING

        return ValidationStatus.PASSING

    def get_validation_geometry(self, world) -> ValidationGeometry:
        """Returns the underlying geometry this validation is checking

        :param world: The world msg to create validation geometry from
        :returns: ValidationGeometry containing geometry to visualize

        """
        return create_validation_geometry(
            [
                tbots_cpp.Circle(self.kick_position, KickerDoubleTouch.VALIDATION_RADIUS_M),
            ] if self.kick_position is not None else []
        )

    def __repr__(self):
        if self.kick_position is not None and self.ball_position is not None:
            return f"Checking if that the kicker robot doesn't double touch from {self.kick_position} "\
                   f"Currently at {self.ball_position}"
        else:
            return "Checking if that the kicker robot doesn't double touch"


(
    _,
    _,
    KickerAlwaysNotDoubleTouch,
    _,
) = create_validation_types(KickerDoubleTouch)
