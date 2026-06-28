/**
 * Cmd Op client node definition.
 *
 * June 28, 2026
 */

/**
 * Copyright 2026 dotX Automation s.r.l.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <atomic>
#include <chrono>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>
#include <tuple>
#include <vector>

#include <rclcpp/rclcpp.hpp>
#include <rclcpp_action/rclcpp_action.hpp>

#include <dua_node_cpp/dua_node.hpp>
#include <dua_qos_cpp/dua_qos.hpp>

#include <dua_common_interfaces/msg/command_result_stamped.hpp>
#include <std_msgs/msg/header.hpp>
#include <std_msgs/msg/string.hpp>

#include <std_srvs/srv/trigger.hpp>

#include <dua_hardware_interfaces/action/arm.hpp>
#include <dua_hardware_interfaces/action/disarm.hpp>

namespace dua_ros_topic_tools
{

using CommandResultStamped = dua_common_interfaces::msg::CommandResultStamped;
using Header               = std_msgs::msg::Header;
using String               = std_msgs::msg::String;

using Trigger              = std_srvs::srv::Trigger;

using Arm                  = dua_hardware_interfaces::action::Arm;
using Disarm               = dua_hardware_interfaces::action::Disarm;

/**
 * Translates operational commands transmitted over a topic into client/server requests.
 */
class CmdOpClient : public dua_node::NodeBase
{
public:
  /**
   * @brief Constructor.
   *
   * @param node_options Node options.
   */
  explicit CmdOpClient(const rclcpp::NodeOptions & node_options = rclcpp::NodeOptions());

  /**
   * @brief Destructor.
   */
  virtual ~CmdOpClient();

private:
  /* Node initialization routines. */
  /**
   * @brief Initializes node parameters.
   */
  void init_parameters() override;

  /**
   * @brief Initializes callback groups.
   */
  void init_cgroups() override;

  /**
   * @brief Initializes subscriptions.
   */
  void init_subscribers() override;

  /**
   * @brief Initializes service clients.
   */
  void init_service_clients() override;

  /**
   * @brief Initializes action clients.
   */
  void init_action_clients() override;

  /* Topic subscription callback groups. */
  rclcpp::CallbackGroup::SharedPtr cmd_op_cgroup_;

  /* Topic subscriptions. */
  rclcpp::Subscription<String>::SharedPtr cmd_op_sub_;

  /* Topic subscription callbacks. */
  /**
   * @brief Processes incoming commands to execute.
   *
   * @param msg Incoming command to parse.
   */
  void cmd_op_sub_clbk(const String::ConstSharedPtr msg);

  /* Service clients. */
  simple_serviceclient::Client<Trigger>::SharedPtr kill_client_;
  simple_serviceclient::Client<Trigger>::SharedPtr reset_client_;

  /* Action clients. */
  simple_actionclient::Client<Arm>::SharedPtr arm_client_;
  simple_actionclient::Client<Disarm>::SharedPtr disarm_client_;

  /* Threads. */
  std::thread op_thread_;

  /* Thread routines. */
  /**
   * @brief Handles an arm operation.
   */
  void handle_arm();

  /**
   * @brief Handles a disarm operation.
   */
  void handle_disarm();

  /**
   * @brief Handles a kill operation.
   */
  void handle_kill();

  /**
   * @brief Handles a reset operation.
   */
  void handle_reset();

  /* Synchronization primitives. */
  std::atomic<bool> operation_in_progress_{false};

  /* Internal state variables. */
  rclcpp::Time                actions_last_ts_     = rclcpp::Time(0, 0, RCL_SYSTEM_TIME);
  rclcpp::Clock               commands_clock_      = rclcpp::Clock(RCL_SYSTEM_TIME);
  rclcpp::Time                services_last_ts_    = rclcpp::Time(0, 0, RCL_SYSTEM_TIME);

  /* Node parameters. */
  std::string actions_arm_name_    = "";
  int64_t     actions_cooldown_    = 0;
  std::string actions_disarm_name_ = "";
  int64_t     services_cooldown_   = 0;
  std::string services_kill_name_  = "";
  std::string services_reset_name_ = "";
  bool        wait_servers_        = false;

  /* Constants. */
  static constexpr std::string_view CMD_ARM    = "arm";
  static constexpr std::string_view CMD_DISARM = "disarm";
  static constexpr std::string_view CMD_KILL   = "kill";
  static constexpr std::string_view CMD_RESET  = "reset";
};

} // namespace dua_ros_topic_tools
