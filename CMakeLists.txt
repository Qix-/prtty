# not required to use prtty
project (prtty)
cmake_minimum_required (VERSION 2.8)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -pedantic -std=c++11 -g3 -O0 -Wno-padded -Wno-shadow -Wno-missing-prototypes")

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weverything -Wno-weak-vtables -Wno-c++98-compat")
endif ()

add_executable (prtty_tests test.cc)
