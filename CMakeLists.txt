cmake_minimum_required(VERSION 2.8)

project(theball-engine)

set(LIBRARY_OUTPUT_PATH ${OUTPUT_PATH})

set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "" FORCE)

set(TBENGINE_BOOST_INCLUDE "" CACHE PATH "TBENGINE BOOST INCLUDE")

include_directories(${TBENGINE_BOOST_INCLUDE})

file(GLOB_RECURSE SOURCES_FILES sources/*)

add_library(tbengine STATIC ${SOURCES_FILES})
