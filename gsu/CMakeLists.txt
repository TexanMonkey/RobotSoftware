cmake_minimum_required(VERSION 2.6 FATAL_ERROR)


set(PROJECT_NAME gsu)
message(STATUS "************  ${PROJECT_NAME} ************")
project(${PROJECT_NAME})

file (GLOB SRCS "src/*.cpp")
file (GLOB HDRS "include/${PROJECT_NAME}/*.h")

include_directories(include)

add_library(${PROJECT_NAME} ${HDRS} ${SRCS})
