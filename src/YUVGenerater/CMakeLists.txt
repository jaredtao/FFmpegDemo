aux_source_directory(. allSrc)
get_filename_component(baseName ${CMAKE_CURRENT_SOURCE_DIR} NAME)
file(GLOB_RECURSE allHeaders  *.h *.hpp)

find_package(libyuv CONFIG REQUIRED)

add_executable(${baseName} ${allSrc} ${allHeaders} ${sharedFiles})

target_link_libraries(${baseName} PRIVATE yuv)

add_test(NAME ${baseName} COMMAND ${baseName})