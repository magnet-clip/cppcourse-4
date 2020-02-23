cmake_minimum_required(VERSION 3.1.0)

project (main)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -Wextra -g -pthread -pg") 

include_directories(${PROJECT_SOURCE_DIR})
add_executable(main utils.cpp 
                    enums.cpp 
                    http_response.cpp 
                    comment_server.cpp 
                    handler_resolver.cpp 
                    http_handler.cpp  
                    main.cpp)
set_property(TARGET main PROPERTY CXX_STANDARD 17)
