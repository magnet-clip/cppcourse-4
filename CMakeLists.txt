cmake_minimum_required(VERSION 3.1.0)

project (main)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -Wall -Wextra -g -pthread -pg") 

include_directories(${PROJECT_SOURCE_DIR})

file(GLOB MAIN_SRC "*.cpp")
file(GLOB TEST_JSON "*.json")
file(COPY ${TEST_JSON} DESTINATION ${CMAKE_BINARY_DIR})

add_executable(main ${MAIN_SRC})

set_property(TARGET main PROPERTY CXX_STANDARD 17)
