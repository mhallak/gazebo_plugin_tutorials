cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

find_package(gazebo REQUIRED)
include_directories(${GAZEBO_INCLUDE_DIRS})
link_directories(${GAZEBO_LIBRARY_DIRS})
list(APPEND CMAKE_CXX_FLAGS "${GAZEBO_CXX_FLAGS}")
#for Gazebo 6 
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GAZEBO_CXX_FLAGS}")
find_library(LIBRT_LIBRARIES rt)
add_library(camera_dump SHARED camera_dump.cc)
#add_library(camera_dump_mic SHARED camera_dump_mic.cc)
target_link_libraries(camera_dump ${GAZEBO_libraries} CameraPlugin  ${LIBRT_LIBRARIES})
#target_link_libraries(camera_dump_mic camera_dump ${GAZEBO_libraries} CameraPlugin  ${LIBRT_LIBRARIES})

add_library(camera_move SHARED camera_move.cc)
target_link_libraries(camera_move ${GAZEBO_libraries})
