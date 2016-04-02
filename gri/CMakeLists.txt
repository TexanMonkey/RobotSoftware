cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

set(PROJECT_NAME gri)
message(STATUS "************  ${PROJECT_NAME} ************")
project(${PROJECT_NAME})

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(PORT_TYPE POSIX)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	set(PORT_TYPE WIN)
	add_definitions( /wd4996 )
endif()

file (GLOB SRCS "src/*.cpp")
file (GLOB HDRS "include/${PROJECT_NAME}/*.h")


include_directories(include)
include_directories(../gsi/include)
include_directories(../gri/include)

add_library(${PROJECT_NAME} ${HDRS} ${SRCS})
target_link_libraries(${PROJECT_NAME} gsi)
