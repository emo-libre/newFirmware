cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# TODO: rename to your app name (lowercase, no spaces, max 15 chars)
project(my-app)

# ESP-IDF components to link into the ELF.
# Only list components whose symbols your app calls directly at compile time.
# Most platform APIs (FreeRTOS, sockets, GPIO, NVS proxy, HTTP server, etc.)
# are resolved at load time from the base firmware's customer symbol table
# and do NOT need to be listed here.
#
# Common additions:
#   "driver"           – gpio_config, gpio_set_level (if not using customer symbols)
#   "esp_driver_rmt"   – RMT / WS2812B
#   "esp_driver_ledc"  – PWM via LEDC
#   "esp_timer"        – esp_timer_* (if not using customer symbols)
set(ELF_COMPONENTS
    # "driver"
)

include(elf_loader)
# TODO: rename to match project() above
project_elf(my-app)
