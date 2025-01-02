package(default_visibility = ["//visibility:public"])

filegroup(
    name = "header_files",
    srcs = glob(["x86_64-linux-gnu/**/*.h"])
)

cc_library(
    name = "headers",
    hdrs = [":header_files"],
    includes = ["."],
)
