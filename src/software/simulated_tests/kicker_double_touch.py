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

    def __init__(self, threshold=0.1):
        """
        :param threshold: The distance from the ball to consider a robot touching it
        """
        self.kicker_robot = None
        self.kick_position = None
        self.other_robot = None
        self.threshold = threshold

    def get_validation_status(self, world) -> ValidationStatus:
        """Checks if the robot kicking the ball touches the ball again before another robot touches it

        :param world: The world msg to validate
        :returns: FAILING when the robot kicking the ball touches the ball again before another robot touches it
                  PASSING when the robot kicking the ball does not touch the ball again before another robot touches it
        """
        ball_position = tbots_cpp.createPoint(world.ball.current_state.global_position)
        if self.kick_position is None:
            self.kick_position = ball_position
        # quick passing check
        if self.other_robot is not None:
            return ValidationStatus.PASSING
        # main check loop
        for robot in world.friendly_team.team_robots:
            if tbots_cpp.Robot(robot).isNearDribbler(ball_position, self.threshold):
                if (
                    self.kicker_robot == robot
                    and self.other_robot is None
                    and (ball_position - self.kick_position).length()
                    > KICKOFF_DOUBLE_TOUCH_M
                ):
                    return ValidationStatus.FAILING
                else:
                    self.other_robot = robot
                    return ValidationStatus.PASSING
        return ValidationStatus.PASSING

    def get_validation_geometry(self, world) -> ValidationGeometry:
        """Returns the underlying geometry this validation is checking

        :param world: The world msg to create validation geometry from
        :returns: ValidationGeometry containing geometry to visualize

        """
        # assigns closest robot to ball as kicker if not already assigned
        kicker_position = world.ball.position()
        closest_robot = world.friendly_team.getNearestRobot(world.ball.position())
        if self.kicker_robot:
            kicker_position = self.kicker_robot.position()
        elif closest_robot:
            kicker_position = closest_robot.position()

        return create_validation_geometry(
            [
                tbots_cpp.Circle(kicker_position, 0.5),
            ]
        )

    def __repr__(self):
        return "Checking that the robot kicking the ball is not touching the ball consecutively"


(
    _,  # not very useful to know that a double-touching robot stops double-touching
    KickerEventuallyDoubleTouch,
    KickerAlwaysNotDoubleTouch,
    _,  # not very useful to know that a robot is always double-touching
) = create_validation_types(KickerDoubleTouch)
