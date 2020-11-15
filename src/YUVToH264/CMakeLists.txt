aux_source_directory(. allSrc)
get_filename_component(baseName ${CMAKE_CURRENT_SOURCE_DIR} NAME)
file(GLOB_RECURSE allHeaders  *.h *.hpp)

if(VCPKG_TARGET_TRIPLET_NOT)
    message("use vcpkg for ffmpeg")
    find_path(AVCODEC_INCLUDE_DIR libavcodec/avcodec.h)
    find_library(AVCODEC_LIBRARY avcodec)

    find_path(AVFORMAT_INCLUDE_DIR libavformat/avformat.h)
    find_library(AVFORMAT_LIBRARY avformat)

    find_path(AVUTIL_INCLUDE_DIR libavutil/avutil.h)
    find_library(AVUTIL_LIBRARY avutil)

    find_path(AVDEVICE_INCLUDE_DIR libavdevice/avdevice.h)
    find_library(AVDEVICE_LIBRARY avdevice)
    include_directories(${AVCODEC_INCLUDE_DIR} ${AVFORMAT_INCLUDE_DIR} ${AVUTIL_INCLUDE_DIR} ${AVDEVICE_INCLUDE_DIR})
    link_directories(${AVCODEC_LIBRARY} ${AVFORMAT_LIBRARY} ${AVUTIL_LIBRARY} ${AVDEVICE_LIBRARY})
else()
    if (WIN32)
        if(CMAKE_CL_64)
            set (FFmpegLibPath ${FFmpegPath}/lib/win64)
        else()
            set (FFmpegLibPath ${FFmpegPath}/lib/win32)
        endif()
    endif()    
    message("FFmpegLibPath ${FFmpegLibPath}")
    link_directories(${FFmpegLibPath} ${EXECUTABLE_OUTPUT_PATH})
    include_directories(SYSTEM ${FFmpegIncludePath})
endif()

add_executable(${baseName} ${allSrc} ${allHeaders} ${sharedFiles})

target_link_libraries(${baseName} PRIVATE avcodec avformat avutil avdevice)

add_test(NAME ${baseName} COMMAND ${baseName})