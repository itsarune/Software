package(default_visibility = ["//visibility:public"])

cc_library(
    name = "motor_index",
    hdrs = ["motor_index.h"],
)

cc_library(
    name = "motor_board",
    hdrs = ["motor_board.h"],
    deps = [
        "//software/util/make_enum:make_enum",
    ]
)

cc_library(
    name = "motor_controller",
    hdrs = [
        "motor_controller.h",
        "stspin_motor_controller.h",
        "tmc_motor_controller.h",
    ],
    srcs = [
        "stspin_motor_controller.cpp",
        "tmc_motor_controller.cpp",
    ],
    deps = [
        ":motor_board",
        ":motor_fault_indicator",
        ":motor_index",
        "//software/embedded:spi_utils",
        "//software/embedded/gpio:gpio",
        "//software/logger:logger",
        "@trinamic",
    ]
)

cc_library(
    name = "motor_fault_indicator",
    srcs = ["motor_fault_indicator.cpp"],
    hdrs = ["motor_fault_indicator.h"],
    deps = [
        ":motor_board",
        "//proto:tbots_cc_proto",
    ]
)
