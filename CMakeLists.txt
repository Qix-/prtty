# not required to use prtty
project (prtty)
cmake_minimum_required (VERSION 2.8)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -pedantic -std=c++11 -g3 -O0 -Wno-c++98-compat -Wno-padded -Wno-shadow -Wno-missing-prototypes -Wno-weak-vtables")

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weverything")
endif ()

add_executable (prtty_tests test.cc)
