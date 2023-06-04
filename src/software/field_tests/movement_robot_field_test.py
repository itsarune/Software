import pytest

import software.python_bindings as tbots
import sys
from proto.import_all_protos import *
from software.field_tests.field_test_fixture import *

from software.logger.logger import createLogger
from software.simulated_tests.robot_enters_region import RobotEventuallyEntersRegion
from proto.message_translation.tbots_protobuf import create_world_state

logger = createLogger(__name__)

# this test can only be run on the field
def test_passing(field_test_runner):
    id = 5

    # current position
    world = field_test_runner.world_buffer.get(block=True, timeout=WORLD_BUFFER_TIMEOUT)
    robot = [robot for robot in world.friendly_team.team_robots if robot.id == id][0]

    rob_pos_p = robot.current_state.global_position
    logger.info("staying in pos {rob_pos_p}")

    pass_generator = tbots.EighteenZoneIdPassGenerator(
        tbots.EighteenZonePitchDivision(tbots.Field.createSSLDivisionBField()),
        PassingConfig(max_receive_speed=py_constants.MAX_PASS_RECEIVE_SPEED),
    )

    for index in range(0, 100):
        pass_eval = pass_generator.generatePassEvaluation(world)
        best_pass_eval = pass_eval.getBestPassOnField()
        best_pass = best_pass_eval.pass_value

    pass_evaluation = pass_generator.generatePassEvaluation(world)
    best_pass_eval = pass_evaluation.getBestPassInZones([tbots.EighteenZoneId.ZONE_4, tbots.EighteenZoneId.EighteenZoneId.ZONE_5, tbots.EighteenZoneId.ZONE_6])
    best_pass = best_pass_eval.pass_value

    kick_vec = tbots.Vector(
        best_pass.receiverPoint().x() - best_pass.passerPoint().x(),
        best_pass.receiverPoint().y() - best_pass.passerPoint().y(),
    )
    # Setup Tactic
    params = AssignedTacticPlayControlParams()
    params.assigned_tactics[0].kick.CopyFrom(
        KickTactic(
            kick_origin=Point(
                x_meters=best_pass.passerPoint().x(),
                y_meters=best_pass.passerPoint().y(),
            ),
            kick_direction=Angle(radians=kick_vec.orientation().toRadians()),
            kick_speed_meters_per_second=best_pass.speed(),
        )
    )

    receiver_args = {
        "pass": Pass(
            passer_point=Point(
                x_meters=best_pass.passerPoint().x(),
                y_meters=best_pass.passerPoint().y(),
            ),
            receiver_point=Point(
                x_meters=best_pass.receiverPoint().x(),
                y_meters=best_pass.receiverPoint().y(),
            ),
            pass_speed_m_per_s=best_pass.speed(),
        ),
        "disable_one_touch_shot": True,
    }

    params.assigned_tactics[1].receiver.CopyFrom(ReceiverTactic(**receiver_args))

    field_test_runner.set_tactics(params, True)
    field_test_runner.run_test(
        always_validation_sequence_set=[[]],
        eventually_validation_sequence_set=[[]],
        test_timeout_s=5,
    )
    # Send a stop tactic after the test finishes
    stop_tactic = StopTactic()
    params = AssignedTacticPlayControlParams()
    params.assigned_tactics[id].stop.CopyFrom(stop_tactic)
    # send the stop tactic
    field_test_runner.set_tactics(params, True)


if __name__ == "__main__":
    # Run the test, -s disables all capturing at -vv increases verbosity
    sys.exit(pytest.main([__file__, "-svv"]))
