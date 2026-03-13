idf_component_register(
    SRCS
        "main.c"
        # TODO: add more .c files here as your app grows
    INCLUDE_DIRS "."
    REQUIRES
        app_api
    PRIV_REQUIRES
        esp_http_server
)

# Prevents "Can't find common SYSTEM" linker errors in ELF apps
target_compile_options(${COMPONENT_LIB} PRIVATE "-fno-common")
