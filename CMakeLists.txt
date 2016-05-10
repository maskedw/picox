# CMake project file

include(mod_picox.cmake)
add_library(picox STATIC ${picox_sources})
