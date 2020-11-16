aux_source_directory(. allSrc)
get_filename_component(baseName ${CMAKE_CURRENT_SOURCE_DIR} NAME)
file(GLOB_RECURSE allHeaders  *.h *.hpp)

message ("FFMPEG_INCLUDE_DIRS ${FFMPEG_INCLUDE_DIRS}")
include_directories(SYSTEM ${FFMPEG_INCLUDE_DIRS})

message ("FFMPEG_LIBRARY_DIRS ${FFMPEG_LIBRARY_DIRS}")
link_directories(${FFMPEG_LIBRARY_DIRS} ${EXECUTABLE_OUTPUT_PATH})

message ("FFMPEG_LIBRARIES ${FFMPEG_LIBRARIES}")
add_executable(${baseName} ${allSrc} ${allHeaders})
target_link_libraries(${baseName} PRIVATE ${FFMPEG_LIBRARIES})

add_test(NAME ${baseName} COMMAND ${baseName})