aux_source_directory(. allSrc)
get_filename_component(baseName ${CMAKE_CURRENT_SOURCE_DIR} NAME)
file(GLOB_RECURSE allHeaders  *.h *.hpp)

# find_package(PkgConfig)
# if (PkgConfig_FOUND) 
#     set (localPkgPath  "E:\Tools\vcpkg\downloads\tools\msys2\8b4e88906af83748\usr\bin\pkg-config.exe")
#     set(ENV{PKG_CONFIG_EXECUTABLE} ${localPkgPath})
#     set(ENV{PKG_CONFIG_PATH} ENV{VCPKG_ROOT}/installed/x64-windows/lib/pkgconfig)
# 	pkg_check_modules(X264 REQUIRED IMPORTED_TARGET x264)
#     set(X264_LIBRARIES x264 CACHE STRING "")
#     set(X264_INCLUDE_DIRS ${PKG_X264_INCLUDE_DIRS} CACHE STRING "")
#     set(X264_LIBRARY_DIRS ${PKG_X264_LIBRARY_DIRS} CACHE STRING "")
# endif()

# message ("X264_INCLUDE_DIRS ${X264_INCLUDE_DIRS}")
# include_directories(SYSTEM ${X264_INCLUDE_DIRS})

# message ("X264_LIBRARY_DIRS ${X264_LIBRARY_DIRS}")
# link_directories(${X264_LIBRARY_DIRS})

# message ("X264_LIBRARIES ${X264_LIBRARIES}")
add_executable(${baseName} ${allSrc} ${allHeaders})
target_link_libraries(${baseName} PRIVATE libx264)

add_test(NAME ${baseName} COMMAND ${baseName})