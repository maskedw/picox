
set(picox_src
   picox/core/detail/debug.c
   picox/filesystem/xfile.c
   picox/filesystem/xpath.c
   picox/allocator/xstack_allocator.c
   picox/allocator/xfixed_allocator.c
   picox/allocator/xpico_allocator.c
   picox/string/xstr.c
   picox/misc/xtokenizer.c
   picox/misc/xargparser.c
)


add_library(picox STATIC
    ${picox_src}
)
