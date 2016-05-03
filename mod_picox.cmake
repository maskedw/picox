# CMake module file

execute_process(COMMAND git rev-parse --show-toplevel
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                OUTPUT_STRIP_TRAILING_WHITESPACE
                OUTPUT_VARIABLE root_dir)

file(TO_CMAKE_PATH ${root_dir}/picox                        picox_dir)
file(TO_CMAKE_PATH ${root_dir}/picox_external               external_dir)
file(TO_CMAKE_PATH ${external_dir}/Unity/port_picox         unity_dir)
file(TO_CMAKE_PATH ${external_dir}/fatfs/port_picox         fatfs_dir)
file(TO_CMAKE_PATH ${external_dir}/spiffs/port_picox        spiffs_dir)
file(TO_CMAKE_PATH ${external_dir}/sds/port_picox           sds_dir)
file(TO_CMAKE_PATH ${external_dir}/uthash/port_picox        uthash_dir)
file(TO_CMAKE_PATH ${external_dir}/minIni/port_picox        minIni_dir)
file(TO_CMAKE_PATH ${external_dir}/libfixmath/port_picox    libfixmath_dir)

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
    ${picox_dir}/core/detail/xstream.c
    ${picox_dir}/core/detail/xstring.c
    ${picox_dir}/core/detail/xrandom.c
    ${picox_dir}/filesystem/xposixfs.c
    ${picox_dir}/filesystem/xfatfs.c
    ${picox_dir}/filesystem/xramfs.c
    ${picox_dir}/filesystem/xromfs.c
    ${picox_dir}/filesystem/xsinglefs.c
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
    ${sds_dir}/sds.c
    ${fatfs_dir}/ff.c
    ${spiffs_dir}/spiffs_cache.c
    ${spiffs_dir}/spiffs_check.c
    ${spiffs_dir}/spiffs_gc.c
    ${spiffs_dir}/spiffs_hydrogen.c
    ${spiffs_dir}/spiffs_nucleus.c
    ${uthash_dir}/libut.c
    ${uthash_dir}/ringbuf.c
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
