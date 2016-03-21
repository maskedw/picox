cmake_minimum_required(VERSION 2.8)
project(picox-tests)

set(CMAKE_BUILD_TYPE Debug)
# set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_C_COMPILER gcc)
set(picox_dir "../picox")
set(external_dir "../picox_external")
set(unity_dir ${external_dir}/Unity/src)
set(unity_ex_dir ${external_dir}/Unity/extras/fixture/src)
set(fatfs_dir ${external_dir}/fatfs/src)
set(spiffs_dir ${external_dir}/spiffs/src)


include_directories("./")
include_directories("../")
include_directories(${external_dir}/sds)
include_directories(${unity_dir})
include_directories(${unity_ex_dir})
include_directories(${fatfs_dir})
include_directories(${spiffs_dir})

# set(CMAKE_C_FLAGS "-std=gnu99 -Wall -Wextra -Wpedantic -DUNITY_INCLUDE_DOUBLE -DX_CONF_USE_USER_CONFIG=1 -include unity.h -include unity_fixture.h")
set(CMAKE_C_FLAGS "-std=gnu99 -Wall -Wextra -DUNITY_INCLUDE_DOUBLE -DX_CONF_USE_USER_CONFIG=1 -include unity.h -include unity_fixture.h")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wchar-subscripts")

add_executable(alltests
    alltests.c
    testutils.c
    test_xstack_allocator.c
    test_xpico_allocator.c
    test_xfixed_allocator.c
    test_xstring.c
    test_xtokenizer.c
    test_xargparser.c
    test_xposixfs.c
    test_xramfs.c
    test_xvfs.c
    test_xfs.c
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
    glue/fatfs_glue.c
    ${picox_dir}/core/detail/xdebug.c
    ${picox_dir}/core/detail/xprintf.c
    ${picox_dir}/core/detail/xstream.c
    ${picox_dir}/core/detail/xstring.c
    ${picox_dir}/core/detail/xrandom.c
    ${picox_dir}/filesystem/xposixfs.c
    ${picox_dir}/filesystem/xfatfs.c
    ${picox_dir}/filesystem/xramfs.c
    ${picox_dir}/filesystem/xvfs.c
    ${picox_dir}/filesystem/xfs.c
    ${picox_dir}/filesystem/xfpath.c
    ${picox_dir}/filesystem/xspiffs.c
    ${picox_dir}/allocator/xstack_allocator.c
    ${picox_dir}/allocator/xfixed_allocator.c
    ${picox_dir}/allocator/xpico_allocator.c
    ${picox_dir}/string/xdynamic_string.c
    ${picox_dir}/misc/xtokenizer.c
    ${picox_dir}/misc/xargparser.c
    ${external_dir}/sds/sds.c
    ${fatfs_dir}/ff.c
    ${spiffs_dir}/spiffs_cache.c
    ${spiffs_dir}/spiffs_check.c
    ${spiffs_dir}/spiffs_gc.c
    ${spiffs_dir}/spiffs_hydrogen.c
    ${spiffs_dir}/spiffs_nucleus.c
    ${unity_dir}/unity.c
    ${unity_ex_dir}/unity_fixture.c)

add_custom_target(run_tests ALL DEPENDS alltests)
add_custom_command(TARGET run_tests POST_BUILD
    COMMAND size ./alltests
    COMMAND ./alltests
    )
