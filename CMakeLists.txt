# not required to use prtty
project (prtty)
cmake_minimum_required (VERSION 2.8)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Weverything -Wextra -Werror -pedantic -std=c++11 -g3 -O0 -Wno-c++98-compat -Wno-padded -Wno-shadow -Wno-missing-prototypes -Wno-weak-vtables")

add_executable (prtty_tests test.cc)
