package(default_visibility = ["//visibility:public"])

cc_library(
    name = "serializer",
    srcs = ["serializer.c"],
    hdrs = ["serializer.h"],
    copts = ["-DLOGGER_ENABLE"],
    visibility = ["//visibility:public"],
    deps = [
        "//accelerator:ta_config",
        "//accelerator:ta_errors",
        "//connectivity/mqtt:mqtt_common",
        "//request",
        "//response",
        "//utils:fill_nines",
        "//utils:ta_logger",
        "@cJSON",
        "@entangled//cclient/response:responses",
        "@entangled//common/trinary:flex_trit",
        "@entangled//common/trinary:tryte_ascii",
        "@entangled//utils:char_buffer",
        "@entangled//utils/containers/hash:hash_array",
    ],
)
