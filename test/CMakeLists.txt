cmake_minimum_required(VERSION 2.8)
project(picox-tests)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_C_COMPILER gcc)
set(PICOX_DIR "../picox")
set(EX_DIR ${PICOX_DIR}/external)
set(UNITY_DIR ${EX_DIR}/Unity/src)
set(UNITY_EX_DIR ${EX_DIR}/Unity/extras/fixture/src)


include_directories("../")
include_directories(${EX_DIR}/sds)
include_directories(${UNITY_DIR})
include_directories(${UNITY_EX_DIR})

set(CMAKE_C_FLAGS "-std=c99 -Wall -Wextra -Wno-sign-compare ")

add_executable(alltests
    alltests.c
    sds_test.c
    xfifo_test.c
    xlist_test.c
    xmessage_buffer_test.c
    ${PICOX_DIR}/filesystem/xfile.c
    ${PICOX_DIR}/allocator/xsalloc.c
    ${PICOX_DIR}/allocator/xfalloc.c
    ${PICOX_DIR}/string/xstr.c
    ${PICOX_DIR}/misc/xdebug.c
    ${PICOX_DIR}/misc/xtokenizer.c
    ${EX_DIR}/sds/sds.c
    ${UNITY_DIR}/unity.c
    ${UNITY_EX_DIR}/unity_fixture.c)


add_custom_command(TARGET alltests POST_BUILD COMMAND ./alltests.exe)
