import sys

import pytest

import software.python_bindings as tbots
from proto.play_pb2 import Play
from proto.message_translation.tbots_protobuf import create_world_state
from proto.ssl_gc_geometry_pb2 import Vector2
from software.simulated_tests.simulated_test_fixture import simulated_test_runner
from proto.import_all_protos import *
from proto.ssl_gc_common_pb2 import Team

def test_kickoff_friendly_play(simulated_test_runner):
    friendly_robots = [
            tbots.Point(-3, 2.5),
            tbots.Point(-3, 1.5),
            tbots.Point(-3, 0.5),
            tbots.Point(-3, -0.5),
            tbots.Point(-3, -1.5),
            tbots.Point(-3, -2.5)
    ]
    
    enemy_robots = [
            tbots.Point(1, 0),
            tbots.Point(1, 2.5),
            tbots.Point(1, -2.5),
            tbots.Field.createSSLDivisionBField().enemyGoalCenter(),
            tbots.Field.createSSLDivisionBField().enemyDefenseArea().negXNegYCorner(),
            tbots.Field.createSSLDivisionBField().enemyDefenseArea().negXPosYCorner()
    ]

    simulated_test_runner.gamecontroller.send_ci_input(
        gc_command=Command.Type.STOP, team=Team.UNKNOWN
    )
    simulated_test_runner.gamecontroller.send_ci_input(
        gc_command=Command.Type.KICKOFF, team=Team.BLUE
    )

    blue_play = Play()
    blue_play.name = Play.KickoffFriendlyPlay

    simulated_test_runner.simulator_proto_unix_io.send_proto(
            WorldState,
            create_world_state(
                yellow_robot_locations=enemy_robots,
                blue_robot_locations=friendly_robots,
                ball_location=tbots.Point(0, 0),
                ball_velocity=tbots.Vector(0, 0),),
    )

    always_validation_sequence_set = [[]]

    eventually_validation_sequence_set =[
        [ 
        ]
    ]

    simulated_test_runner.run_test(
            eventually_validation_sequence_set=eventually_validation_sequence_set,
            always_validation_sequence_set=always_validation_sequence_set,
            test_timeout_s=10,
    )

if __name__ == "__main__":
    # Run the test, -s disables all capturing at -vv increases verbosity
    sys.exit(pytest.main([__file__, "-svv"]))
