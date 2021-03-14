cmake_minimum_required(VERSION 3.13.1)

set(DTS_ROOT ${CMAKE_CURRENT_LIST_DIR})
set(DTC_OVERLAY_FILE ${CMAKE_CURRENT_LIST_DIR}/stepper_config.overlay)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(zerhacker)

target_sources(app PRIVATE src/main.c
                           src/stepper.c
                           src/zerhacker.c
                           src/shell.c)
