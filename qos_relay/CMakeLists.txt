cmake_minimum_required(VERSION 3.12)
set(BUILD_TESTING OFF CACHE BOOL "Build tests")
project(qos_relay)

set(CMAKE_BUILD_TYPE "RelWithDebInfo")

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
find_package(dua_node_cpp REQUIRED)
find_package(dua_qos_cpp REQUIRED)
find_package(params_manager_cpp REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_components REQUIRED)
find_package(rmw REQUIRED)

# Generate parameters source code
generate_init_parameters(
  YAML_FILE "${CMAKE_CURRENT_SOURCE_DIR}/src/qos_relay/params.yaml"
  OUT_FILE "init_parameters.cpp")

# Configure node source files
file(GLOB QOS_RELAY_SRCS
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" CONFIGURE_DEPENDS
  "src/qos_relay/*.cpp")
list(APPEND QOS_RELAY_SRCS init_parameters.cpp)

# Composable node
add_library(qos_relay SHARED ${QOS_RELAY_SRCS})
target_compile_definitions(qos_relay PRIVATE COMPOSITION_BUILDING_DLL)
target_include_directories(qos_relay PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include/${PROJECT_NAME}>)
target_compile_features(qos_relay PUBLIC c_std_99 cxx_std_17) # Require C99 and C++17
ament_target_dependencies(qos_relay
  dua_node_cpp
  dua_qos_cpp
  params_manager_cpp
  rclcpp
  rclcpp_components
  rmw)
rclcpp_components_register_nodes(qos_relay "dua_ros_topic_tools::QoSRelay")

# Standalone application
add_executable(qos_relay_app
  src/qos_relay_app.cpp)
target_include_directories(qos_relay_app PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include/${PROJECT_NAME}>)
target_compile_features(qos_relay_app PUBLIC c_std_99 cxx_std_17) # Require C99 and C++17
target_link_libraries(qos_relay_app qos_relay)
ament_target_dependencies(qos_relay_app
  dua_app_management
  rclcpp)

# Install composable node
install(TARGETS qos_relay
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)

# Install app
install(TARGETS qos_relay_app
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
