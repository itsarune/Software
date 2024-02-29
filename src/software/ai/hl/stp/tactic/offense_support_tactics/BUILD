package(default_visibility = ["//visibility:public"])

cc_library(
    name = "cherry_picker_tactic",
    hdrs = ["cherry_picker_tactic.h"],
    deps = [
        "//software/ai/hl/stp/tactic/offense_support_tactics/receiver:receiver_tactic"
    ],
)

cc_library(
    name = "offense_support_tactic",
    hdrs = ["offense_support_tactic.h"],
    srcs = ["offense_support_tactic.cpp"],
    deps = [
        "//software/ai/hl/stp/tactic:tactic",
        "//software/util/make_enum",
    ],
)
