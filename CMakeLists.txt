cmake_minimum_required(VERSION 2.8.3)
project(heartbeat)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -c -std=c++11")
#set(CMAKE_BUILD_TYPE Release)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  std_msgs
  message_generation
)


add_message_files(
  FILES
  Running.msg
)

generate_messages(
  DEPENDENCIES
  std_msgs
)

catkin_package(
	CATKIN_DEPENDS
#  INCLUDE_DIRS include
#  LIBRARIES heartbeat
#  CATKIN_DEPENDS roscpp rospy std_msgs
#  DEPENDS system_lib
)

include_directories(include/heartbeat)
include_directories(
  ${catkin_INCLUDE_DIRS}
)

set (RESTCLIENT_LIB ${PROJECT_SOURCE_DIR}/include/restclient-cpp/.libs/librestclient-cpp.so)

add_executable(heartbeat src/heartbeat.cpp)
target_link_libraries(heartbeat ${catkin_LIBRARIES} ${RESTCLIENT_LIB})
add_dependencies(heartbeat ${catkin_EXPORTED_TARGETS})
add_dependencies(heartbeat navigation_generate_messages_cpp)
