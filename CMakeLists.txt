cmake_minimum_required(VERSION 2.4.6)
#include(CMakeMacros.cmake)

set (PROJECT_NAME TOP)
message(STATUS "************  Starting ${PROJECT_NAME}  ************")
project(${PROJECT_NAME})
add_definitions(-DPTHREADS)
add_definitions(-DLINUX)
set( CMAKE_BUILD_TYPE 	Debug	)

#if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
#	# anything platform dependant
#elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
#	# anything platform dependant
#endif()

message(STATUS "...............Doing Stuff...............")

#add_subdirectory(gsu)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/../bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/../lib)
link_directories(${LIBRARY_OUTPUT_PATH})
message(STATUS "Directory is ${PROJECT_BINARY_DIR}")
message(STATUS "Binary directory is ${EXECUTABLE_OUTPUT_PATH}")
message(STATUS "Library directory is ${LIBRARY_OUTPUT_PATH}")


add_subdirectory(gsi)
add_subdirectory(gri)
add_subdirectory(TestRobot)
#
#
#
