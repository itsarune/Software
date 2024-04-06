package(default_visibility = ["//visibility:public"])

load("@thunderscope_deps//:requirements.bzl", "requirement")

py_library(
    name = "proto_streamer_widget",
    srcs = ["proto_streamer_widget.py"],
    deps = [
        "//proto:software_py_proto",
        "//software/thunderscope:thread_safe_buffer",
        requirement("pyqtgraph"),
    ],
)
