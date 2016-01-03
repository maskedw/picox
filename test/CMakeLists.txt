cmake_minimum_required(VERSION 2.8)
project(picox-tests)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_C_COMPILER gcc)
set(picox_dir "../picox")
set(external_dir "../picox/external")
set(unity_dir ${external_dir}/Unity/src)
set(unity_ex_dir ${external_dir}/Unity/extras/fixture/src)


include_directories("./")
include_directories("../")
include_directories(${external_dir}/sds)
include_directories(${unity_dir})
include_directories(${unity_ex_dir})

set(CMAKE_C_FLAGS "-std=gnu99 -Wall -Wextra -DUNITY_INCLUDE_DOUBLE -DX_CONF_USE_USER_CONFIG=1 -include unity.h -include unity_fixture.h")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wchar-subscripts")

add_executable(alltests
    alltests.c
    testutils.c
    test_xstack_allocator.c
    test_xpico_allocator.c
    test_xfixed_allocator.c
    test_xstr.c
    test_xtokenizer.c
    test_xargparser.c
    test_xfile.c
    test_sds.c
    test_xfifo_buffer.c
    test_xmessage_buffer.c
    test_xintrusive_list.c
    test_xutils.c
    test_xprintf.c
    test_xdynamic_string.c
    ${picox_dir}/core/detail/debug.c
    ${picox_dir}/core/detail/printf.c
    ${picox_dir}/filesystem/xfile.c
    ${picox_dir}/allocator/xstack_allocator.c
    ${picox_dir}/allocator/xfixed_allocator.c
    ${picox_dir}/allocator/xpico_allocator.c
    ${picox_dir}/string/xstr.c
    ${picox_dir}/string/xdynamic_string.c
    ${picox_dir}/misc/xtokenizer.c
    ${picox_dir}/misc/xargparser.c
    ${external_dir}/sds/sds.c
    ${unity_dir}/unity.c
    ${unity_ex_dir}/unity_fixture.c)

add_custom_target(run_tests ALL DEPENDS alltests)
add_custom_command(TARGET run_tests POST_BUILD COMMAND ./alltests)
