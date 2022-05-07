package(default_visibility = ["//visibility:public"])

# We force linking for all plays so that the static variables required for the
# "factory" design pattern to work are linked in
# https://www.bfilipek.com/2018/02/static-vars-static-lib.html

cc_library(
    name = "kickoff_friendly_play",
    srcs = [
        "kickoff_friendly_play.cpp",
        "kickoff_friendly_play_fsm.cpp",
    ],
    hdrs = [
        "kickoff_friendly_play.h",
        "kickoff_friendly_play_fsm.h",
    ],
    deps = [
        "//software/ai/hl/stp/play",
        "//software/ai/hl/stp/tactic/move:move_tactic",
        "//software/ai/hl/stp/play/crease_defense:crease_defense_play",
        "//software/ai/hl/stp/play/shoot_or_pass:shoot_or_pass_play",
        "//software/ai/hl/stp/tactic:all_tactics"
    ],
    alwayslink = True,
)

cc_test(
    name = "kickoff_friendly_play_test",
    srcs = ["kickoff_friendly_play_test.cpp"],
    deps = [
        ":kickoff_friendly_play"
    ],
)

cc_test(
    name = "kickoff_friendly_play_fsm_test",
    srcs = ["kickoff_friendly_play_fsm_test.cpp"],
    deps = [
        ":kickoff_friendly_play",
		"//software/test_util:test_util",
        "//shared/parameter:cpp_configs",
    ],
)
