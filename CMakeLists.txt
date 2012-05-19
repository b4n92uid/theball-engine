cmake_minimum_required(VERSION 2.6)

project(theBallEngine)

set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/${CMAKE_BUILD_TYPE})

include_directories(${DEVIL_INCLUDE})

file(GLOB_RECURSE SOURCES_FILES sources/*)

add_library(tbengine STATIC ${SOURCES_FILES})
