# QMake project file

TARGET = picox_tests

root_dir            = $$system(git rev-parse --show-toplevel)
picox_dir           = $$root_dir/picox
external_dir        = $$root_dir/picox_external
external_config_dir = $$external_dir/config
unity_dir           = $$external_dir/Unity/port_picox
fatfs_dir           = $$external_dir/fatfs/port_picox
spiffs_dir          = $$external_dir/spiffs/port_picox
sds_dir             = $$external_dir/sds/port_picox
uthash_dir          = $$external_dir/uthash/port_picox
minIni_dir          = $$external_dir/minIni/port_picox
libfixmath_dir      = $$external_dir/libfixmath/port_picox

DEFINES += X_CONF_USE_USER_CONFIG=1
DEFINES += UNITY_INCLUDE_DOUBLE

INCLUDEPATH += ./
INCLUDEPATH += ./config
INCLUDEPATH += ./Build
INCLUDEPATH += $$root_dir
INCLUDEPATH += $$external_dir
INCLUDEPATH += $$external_config_dir
INCLUDEPATH += $$unity_dir
INCLUDEPATH += $$fatfs_dir
INCLUDEPATH += $$spiffs_dir
INCLUDEPATH += $$sds_dir
INCLUDEPATH += $$uthash_dir
INCLUDEPATH += $$minIni_dir
INCLUDEPATH += $$libfixmath_dir

SOURCES += $$picox_dir/core/detail/xdebug.c
SOURCES += $$picox_dir/core/detail/xstdio.c
SOURCES += $$picox_dir/core/detail/xstream.c
SOURCES += $$picox_dir/core/detail/xstring.c
SOURCES += $$picox_dir/core/detail/xrandom.c
SOURCES += $$picox_dir/filesystem/xposixfs.c
SOURCES += $$picox_dir/filesystem/xfatfs.c
SOURCES += $$picox_dir/filesystem/xramfs.c
SOURCES += $$picox_dir/filesystem/xromfs.c
SOURCES += $$picox_dir/filesystem/xvfs.c
SOURCES += $$picox_dir/filesystem/xsinglefs.c
SOURCES += $$picox_dir/filesystem/xunionfs.c
SOURCES += $$picox_dir/filesystem/xfpath.c
SOURCES += $$picox_dir/filesystem/xspiffs.c
SOURCES += $$picox_dir/allocator/xstack_allocator.c
SOURCES += $$picox_dir/allocator/xfixed_allocator.c
SOURCES += $$picox_dir/allocator/xpico_allocator.c
SOURCES += $$picox_dir/string/xdynamic_string.c
SOURCES += $$picox_dir/misc/xtokenizer.c
SOURCES += $$picox_dir/misc/xargparser.c
SOURCES += $$picox_dir/multitask/xfiber.c
SOURCES += $$sds_dir/sds.c
SOURCES += $$fatfs_dir/ff.c
SOURCES += $$spiffs_dir/spiffs_cache.c
SOURCES += $$spiffs_dir/spiffs_check.c
SOURCES += $$spiffs_dir/spiffs_gc.c
SOURCES += $$spiffs_dir/spiffs_hydrogen.c
SOURCES += $$spiffs_dir/spiffs_nucleus.c
SOURCES += $$uthash_dir/libut.c
SOURCES += $$uthash_dir/ringbuf.c
SOURCES += $$uthash_dir/utmm.c
SOURCES += $$uthash_dir/utvector.c
SOURCES += $$minIni_dir/minIni.c
SOURCES += $$minIni_dir/minGlue.c
SOURCES += $$unity_dir/unity.c
SOURCES += $$unity_dir/unity_fixture.c
SOURCES += $$libfixmath_dir/fix16.c
SOURCES += $$libfixmath_dir/fix16_exp.c
SOURCES += $$libfixmath_dir/fix16_sqrt.c
SOURCES += $$libfixmath_dir/fix16_str.c
SOURCES += $$libfixmath_dir/fix16_trig.c
SOURCES += $$libfixmath_dir/fract32.c
SOURCES += $$libfixmath_dir/uint32.c

HEADERS += $$picox_dir/allocator/xfixed_allocator.h
HEADERS += $$picox_dir/allocator/xpico_allocator.h
HEADERS += $$picox_dir/allocator/xstack_allocator.h
HEADERS += $$picox_dir/container/xbyte_array.h
HEADERS += $$picox_dir/container/xfifo_buffer.h
HEADERS += $$picox_dir/container/xintrusive_list.h
HEADERS += $$picox_dir/container/xmessage_buffer.h
HEADERS += $$picox_dir/core/detail/compiler/xgcc.h
HEADERS += $$picox_dir/core/detail/compiler/xrenesas.h
HEADERS += $$picox_dir/core/detail/xcompiler.h
HEADERS += $$picox_dir/core/detail/xdebug.h
HEADERS += $$picox_dir/core/detail/xpreprocessor.h
HEADERS += $$picox_dir/core/detail/xrandom.h
HEADERS += $$picox_dir/core/detail/xstddef.h
HEADERS += $$picox_dir/core/detail/xstdio.h
HEADERS += $$picox_dir/core/detail/xstdlib.h
HEADERS += $$picox_dir/core/detail/xstream.h
HEADERS += $$picox_dir/core/detail/xstring.h
HEADERS += $$picox_dir/core/detail/xtime.h
HEADERS += $$picox_dir/core/detail/xutils.h
HEADERS += $$picox_dir/core/xcore.h
HEADERS += $$picox_dir/filesystem/xfatfs.h
HEADERS += $$picox_dir/filesystem/xfpath.h
HEADERS += $$picox_dir/filesystem/xunionfs.h
HEADERS += $$picox_dir/filesystem/xfscore.h
HEADERS += $$picox_dir/filesystem/xfs.h
HEADERS += $$picox_dir/filesystem/xposixfs.h
HEADERS += $$picox_dir/filesystem/xramfs.h
HEADERS += $$picox_dir/filesystem/xromfs.h
HEADERS += $$picox_dir/filesystem/xspiffs.h
HEADERS += $$picox_dir/filesystem/xvfs.h
HEADERS += $$picox_dir/filesystem/xsinglefs.h
HEADERS += $$picox_dir/misc/xargparser.h
HEADERS += $$picox_dir/misc/xtokenizer.h
HEADERS += $$picox_dir/string/xdynamic_string.h
HEADERS += $$picox_dir/multitask/xfiber.h
HEADERS += $$picox_dir/xconfig.h

SOURCES += ./picox_tests.c
SOURCES += ./testutils.c
SOURCES += ./Build/romfsimg.c
SOURCES += ./test_xstack_allocator.c
SOURCES += ./test_xpico_allocator.c
SOURCES += ./test_xfixed_allocator.c
SOURCES += ./test_xstring.c
SOURCES += ./test_xtokenizer.c
SOURCES += ./test_xargparser.c
SOURCES += ./test_xposixfs.c
SOURCES += ./test_xramfs.c
SOURCES += ./test_xromfs.c
SOURCES += ./test_xfs.c
SOURCES += ./test_xvfs.c
SOURCES += ./test_xsinglefs.c
SOURCES += ./test_xunionfs.c
SOURCES += ./test_xfatfs.c
SOURCES += ./test_xspiffs.c
SOURCES += ./test_xfpath.c
SOURCES += ./test_sds.c
SOURCES += ./test_xfifo_buffer.c
SOURCES += ./test_xmessage_buffer.c
SOURCES += ./test_xintrusive_list.c
SOURCES += ./test_xutils.c
SOURCES += ./test_xprintf.c
SOURCES += ./test_xdynamic_string.c
SOURCES += ./test_xstream.c
SOURCES += ./test_minIni.c
SOURCES += ./test_xfiber.c
SOURCES += ./glue/fatfs_glue.c
