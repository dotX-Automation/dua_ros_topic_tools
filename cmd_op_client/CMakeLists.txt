cmake_minimum_required(VERSION 3.12)
set(BUILD_TESTING OFF CACHE BOOL "Build tests")
project(cmd_op_client)

# Default to C99
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()

# Default to C++17
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(THREADS_PREFER_PTHREAD_FLAG ON)

# Find dependencies
find_package(ament_cmake REQUIRED)
find_package(dua_app_management REQUIRED)
find_package(dua_common_interfaces REQUIRED)
find_package(dua_hardware_interfaces REQUIRED)
find_package(dua_node_cpp REQUIRED)
find_package(dua_qos_cpp REQUIRED)
find_package(params_manager_cpp REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_action REQUIRED)
find_package(rclcpp_components REQUIRED)
find_package(simple_actionclient_cpp REQUIRED)
find_package(simple_serviceclient_cpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(std_srvs REQUIRED)
find_package(Threads REQUIRED)

# Generate parameters source code
generate_init_parameters(
  YAML_FILE "${CMAKE_CURRENT_SOURCE_DIR}/src/cmd_op_client/params.yaml"
  OUT_FILE "init_parameters.cpp")

# Configure node source files
file(GLOB CMD_OP_CLIENT_SRCS
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" CONFIGURE_DEPENDS
  "src/cmd_op_client/*.cpp")
list(APPEND CMD_OP_CLIENT_SRCS init_parameters.cpp)

# Composable node
add_library(cmd_op_client SHARED ${CMD_OP_CLIENT_SRCS})
target_compile_definitions(cmd_op_client PRIVATE COMPOSITION_BUILDING_DLL)
target_include_directories(cmd_op_client PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include/${PROJECT_NAME}>)
target_compile_features(cmd_op_client PUBLIC c_std_99 cxx_std_17) # Require C99 and C++17
target_link_libraries(cmd_op_client
  Threads::Threads)
ament_target_dependencies(cmd_op_client
  dua_app_management
  dua_common_interfaces
  dua_hardware_interfaces
  dua_node_cpp
  dua_qos_cpp
  params_manager_cpp
  rclcpp
  rclcpp_action
  rclcpp_components
  simple_actionclient_cpp
  simple_serviceclient_cpp
  std_msgs
  std_srvs)
rclcpp_components_register_nodes(cmd_op_client "dua_ros_topic_tools::CmdOpClient")

# Standalone application
add_executable(cmd_op_client_app
  src/cmd_op_client_app.cpp)
target_include_directories(cmd_op_client_app PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include/${PROJECT_NAME}>)
target_compile_features(cmd_op_client_app PUBLIC c_std_99 cxx_std_17) # Require C99 and C++17
target_link_libraries(cmd_op_client_app cmd_op_client)
ament_target_dependencies(cmd_op_client_app
  dua_app_management
  rclcpp)

# Install composable node
install(TARGETS cmd_op_client
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)

# Install application
install(TARGETS cmd_op_client_app
  DESTINATION lib/${PROJECT_NAME})

# Install Config files
install(DIRECTORY config
  DESTINATION share/${PROJECT_NAME})

# Install Launch files
install(DIRECTORY launch
  DESTINATION share/${PROJECT_NAME})

if(BUILD_TESTING)
  # Enable coverage instrumentation
  add_compile_options(--coverage -O0 -g)
  add_link_options(--coverage)

  # Static analysis
  find_program(CPPCHECK cppcheck)
  if(CPPCHECK)
    add_test(
      NAME cppcheck
      COMMAND ${CPPCHECK}
        --enable=warning,style,performance,portability
        --suppress=missingIncludeSystem
        --inline-suppr
        --error-exitcode=1
        -I ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  endif()
endif()

ament_package()
