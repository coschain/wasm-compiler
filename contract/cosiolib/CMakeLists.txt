cmake_minimum_required(VERSION 3.1)
project(cosiolib)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Boost 1.50 REQUIRED)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    add_executable(cosiolib main.cpp dummy_system.c)
    target_link_libraries(cosiolib ${Boost_LIBRARIES})
endif()

