package(default_visibility = ["//visibility:public"])

load("@bazel_skylib//rules:common_settings.bzl", "string_flag")

string_flag(
    name = "motor_board",
    build_setting_default = "TRINAMIC_MOTOR_BOARD",
    values = [
        "TRINAMIC_MOTOR_BOARD",
        "STSPIN_MOTOR_BOARD",
    ],
)

config_setting(
    name = "trinamic",
    flag_values = {
        ":motor_board": "TRINAMIC_MOTOR_BOARD",
    },
)

config_setting(
    name = "stspin",
    flag_values = {
        ":motor_board": "STSPIN_MOTOR_BOARD",
    },
)

cc_library(
    name = "motor",
    srcs = ["motor.cpp"],
    hdrs = ["motor.h"],
    defines =
        select({
            "//software/embedded:build_nano": ["NANO"],
            "//software/embedded:build_pi": ["PI"],
            "//software/embedded:build_limited": ["LIMITED"],
        }) +
        select({
            ":trinamic": ["TRINAMIC_MOTOR_BOARD"],
            ":stspin": ["STSPIN_MOTOR_BOARD"],
        }),
    deps = [
        "//proto:tbots_cc_proto",
        "//shared:robot_constants",
        "//software/embedded/gpio:gpio",
        "//software/embedded/constants",
        "//software/embedded/motor_controller:motor_board",
        "//software/embedded/motor_controller:motor_controller",
        "//software/embedded/motor_controller:motor_fault_indicator",
        "//software/logger",
        "//software/physics:euclidean_to_wheel",
        "//software/util/scoped_timespec_timer",
        "@eigen",
        "@trinamic",
    ],
)

cc_library(
    name = "power",
    srcs = ["power.cpp"],
    hdrs = ["power.h"],
    linkopts = ["-latomic"],  # Necessary due to <atomic> not containing load/store for all sizes
    deps = [
        "//shared/uart_framing",
        "//software/logger",
        "//software/uart:boost_uart_communication",
        "@boost//:asio",
        "@boost//:filesystem",
    ],
)

cc_binary(
    name = "robot_auto_test",
    srcs = ["robot_auto_test.cpp"],
    linkopts = [
        "-lpthread",
        "-lrt",
    ],
    deps = [
        ":motor",
        ":power",
        "//proto/message_translation:tbots_geometry",
        "//proto/primitive:primitive_msg_factory",
        "//shared:robot_constants",
        "//software/embedded:primitive_executor",
        "//software/logger",
        "@trinamic",
    ],
)
