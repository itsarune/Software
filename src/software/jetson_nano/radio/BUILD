package(default_visibility = ["//visibility:public"])

load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "sx1280_hal",
    hdrs = ["sx1280_hal.h"],
    srcs = ["sx1280_hal.cpp"],
    deps = [":sx1280_spidev"],
)

cc_library(
    name = "sx1280_spidev",
    hdrs = ["sx1280_spidev.h"],
    srcs = ["sx1280_spidev.cpp"],
    deps = [
        "//software/logger:logger",
    ],
)
