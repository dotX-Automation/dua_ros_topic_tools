cmake_minimum_required(VERSION 3.12)
project(dua_ros_topic_tools)

find_package(ament_cmake REQUIRED)
find_package(cmd_op_client REQUIRED)
find_package(qos_relay REQUIRED)

ament_export_dependencies(
  cmd_op_client
  qos_relay
)

ament_package()
