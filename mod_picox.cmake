# CMake module file

set(root_dir        ${CMAKE_CURRENT_LIST_DIR})
set(picox_dir       "${root_dir}/picox")
set(external_dir    "${root_dir}/picox_external")
set(unity_dir       "${external_dir}/Unity")
set(fatfs_dir       "${external_dir}/fatfs")
set(spiffs_dir      "${external_dir}/spiffs")
set(sds_dir         "${external_dir}/sds")
set(uthash_dir      "${external_dir}/uthash")
set(minIni_dir      "${external_dir}/minIni")
set(libfixmath_dir  "${external_dir}/libfixmath")

include_directories(${root_dir})
include_directories(${unity_dir})
include_directories(${fatfs_dir})
include_directories(${spiffs_dir})
include_directories(${sds_dir})
include_directories(${minIni_dir})
include_directories(${libfixmath_dir})

set(picox_sources
    ${picox_dir}/core/detail/xdebug.c
    ${picox_dir}/core/detail/xstdio.c
    ${picox_dir}/core/detail/xstdlib.c
    ${picox_dir}/core/detail/xstream.c
    ${picox_dir}/core/detail/xstring.c
    ${picox_dir}/core/detail/xrandom.c
    ${picox_dir}/core/detail/xtime.c
    ${picox_dir}/core/detail/xutils.c
    ${picox_dir}/core/xmemstream.c
    ${picox_dir}/container/xintrusive_list.c
    ${picox_dir}/container/xfifo_buffer.c
    ${picox_dir}/filesystem/xfscore.c
    ${picox_dir}/filesystem/xposixfs.c
    ${picox_dir}/filesystem/xfatfs.c
    ${picox_dir}/filesystem/xramfs.c
    ${picox_dir}/filesystem/xromfs.c
    ${picox_dir}/filesystem/xsinglefs.c
    ${picox_dir}/filesystem/xvfs.c
    ${picox_dir}/filesystem/xunionfs.c
    ${picox_dir}/filesystem/xfpath.c
    ${picox_dir}/filesystem/xspiffs.c
    ${picox_dir}/allocator/xstack_allocator.c
    ${picox_dir}/allocator/xfixed_allocator.c
    ${picox_dir}/allocator/xpico_allocator.c
    ${picox_dir}/string/xdynamic_string.c
    ${picox_dir}/misc/xtokenizer.c
    ${picox_dir}/misc/xargparser.c
    ${picox_dir}/multitask/xfiber.c
    ${picox_dir}/multitask/xvtimer.c
    ${picox_dir}/hal/xgpio.c
    ${picox_dir}/hal/xuart.c
    ${picox_dir}/hal/xspi.c
    ${picox_dir}/hal/xi2c.c
    ${picox_dir}/hal/xpwm.c
    ${sds_dir}/sds.c
    ${fatfs_dir}/ff.c
    ${spiffs_dir}/spiffs_cache.c
    ${spiffs_dir}/spiffs_check.c
    ${spiffs_dir}/spiffs_gc.c
    ${spiffs_dir}/spiffs_hydrogen.c
    ${spiffs_dir}/spiffs_nucleus.c
    ${uthash_dir}/libut.c
    ${uthash_dir}/utmm.c
    ${uthash_dir}/utvector.c
    ${minIni_dir}/minIni.c
    ${minIni_dir}/minGlue.c
    ${unity_dir}/unity.c
    ${unity_dir}/unity_fixture.c
    ${libfixmath_dir}/fix16.c
    ${libfixmath_dir}/fix16_exp.c
    ${libfixmath_dir}/fix16_sqrt.c
    ${libfixmath_dir}/fix16_str.c
    ${libfixmath_dir}/fix16_trig.c
    ${libfixmath_dir}/fract32.c
    ${libfixmath_dir}/uint32.c
)
