include_directories(SYSTEM ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/shared)
add_subdirectory(shared)
add_subdirectory(YUVGenerater)
add_subdirectory(YUVToH264-FFmpeg)
add_subdirectory(YUVToH264-Libx264)

# add_subdirectory(YUVFromH264)
