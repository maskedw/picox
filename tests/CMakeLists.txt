cmake_minimum_required(VERSION 2.8)
project(picox-tests)

set(CMAKE_BUILD_TYPE    Debug)
# set(CMAKE_BUILD_TYPE  Release)
set(CMAKE_CXX_COMPILER  g++)
set(CMAKE_C_COMPILER    gcc)

set(rootdir ${CMAKE_SOURCE_DIR}/..)
set(tooldir ${rootdir}/tools)
set(CMAKE_MODULE_PATH ${rootdir})
include(mod_picox)

set(config_dir ${CMAKE_SOURCE_DIR}/config)

include_directories(./)
include_directories(./config)
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_BINARY_DIR})
set(CMAKE_C_FLAGS "-std=c99 -Wall -Wextra -Wpedantic -Wchar-subscripts")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-result")

remove_definitions(
)
add_definitions(
    -DX_CONF_USE_USER_CONFIG=1
    -D_POSIX_C_SOURCE=200809L
    -DX_INTERNAL_DEVMODE
    -DUNITY_INCLUDE_DOUBLE
)

set(test_sources
    picox_tests.c
    testutils.c
    test_xstack_allocator.c
    test_xpico_allocator.c
    test_xfixed_allocator.c
    test_xstring.c
    test_xtokenizer.c
    test_xargparser.c
    test_xposixfs.c
    test_xramfs.c
    test_xromfs.c
    test_xfs.c
    test_xvfs.c
    test_xsinglefs.c
    test_xunionfs.c
    test_xfatfs.c
    test_xspiffs.c
    test_xfpath.c
    test_sds.c
    test_xfifo_buffer.c
    test_xmessage_buffer.c
    test_xintrusive_list.c
    test_xutils.c
    test_xprintf.c
    test_xdynamic_string.c
    test_xstream.c
    test_minIni.c
    test_xfiber.c
    test_xfiber_event.c
    test_xfiber_signal.c
    test_xfiber_mutex.c
    test_xfiber_semaphore.c
    test_xfiber_pool.c
    test_xfiber_mailbox.c
    test_xfiber_channel.c
    test_xfiber_queue.c
    romfsimg.c
    glue/fatfs_glue.c
)

add_library(picox STATIC ${picox_sources})
add_executable(picox_tests ${test_sources})
target_link_libraries(picox_tests picox)

add_custom_command(OUTPUT romfsimg.c romfsimg.h
    COMMAND python3 ${tooldir}/xromfs_builder.py -o romfs.img ${CMAKE_SOURCE_DIR}/romfs
    COMMAND python3 ${tooldir}/xbin2c.py -o romfsimg romfs.img
)

add_custom_target(generate_romfs DEPENDS romfsimg.c romfsimg.h)
add_dependencies(picox_tests generate_romfs)
add_dependencies(picox_tests picox)


# add_custom_command(TARGET picox_tests POST_BUILD
#     COMMAND size ./picox_tests
#     COMMAND ./picox_tests
#     )

add_custom_target(run_tests ALL DEPENDS picox_tests)
add_custom_command(TARGET run_tests POST_BUILD
    COMMAND size ./$<TARGET_FILE_NAME:picox_tests>
    COMMAND ./$<TARGET_FILE_NAME:picox_tests>
    )
