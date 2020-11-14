aux_source_directory(. allSrc)
file(GLOB_RECURSE allHeaders  *.h *.hpp)

set(sharedSourcePath ${CMAKE_CURRENT_SOURCE_DIR} CACHE STRING "sharedSourcePath")
set(sharedFiles ${allSrc} ${allHeaders} CACHE STRING "sharedFiles")