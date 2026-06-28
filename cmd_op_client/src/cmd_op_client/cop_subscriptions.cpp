/**
 * Cmd Op client node topic subscriptions.
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

#include <cmd_op_client/cmd_op_client.hpp>

namespace dua_ros_topic_tools
{

void CmdOpClient::cmd_op_sub_clbk(const String::ConstSharedPtr msg)
{
  // Check if an operation is in progress
  if (operation_in_progress_.load(std::memory_order_acquire)) {
    return;
  }

  // Clean up previous executions
  if (op_thread_.joinable()) {
    op_thread_.join();
  }

  std::string cmd = msg->data;
  rclcpp::Time now_ts = commands_clock_.now();

  // Process actions
  if (now_ts - actions_last_ts_ > rclcpp::Duration(std::chrono::nanoseconds(actions_cooldown_ * 1000000))) {
    if (cmd == CMD_ARM && !actions_arm_name_.empty()) {
      operation_in_progress_.store(true, std::memory_order_release);
      op_thread_ = std::thread(
        std::bind(
          &CmdOpClient::handle_arm,
          this));
      actions_last_ts_ = commands_clock_.now();
    }

    if (cmd == CMD_DISARM && !actions_disarm_name_.empty()) {
      operation_in_progress_.store(true, std::memory_order_release);
      op_thread_ = std::thread(
        std::bind(
          &CmdOpClient::handle_disarm,
          this));
      actions_last_ts_ = commands_clock_.now();
    }
  }

  // Process services
  if (now_ts - services_last_ts_ > rclcpp::Duration(std::chrono::nanoseconds(services_cooldown_ * 1000000))) {
    if (cmd == CMD_KILL && !services_kill_name_.empty()) {
      operation_in_progress_.store(true, std::memory_order_release);
      op_thread_ = std::thread(
        std::bind(
          &CmdOpClient::handle_kill,
          this));
      services_last_ts_ = commands_clock_.now();
    }

    if (cmd == CMD_RESET && !services_reset_name_.empty()) {
      operation_in_progress_.store(true, std::memory_order_release);
      op_thread_ = std::thread(
        std::bind(
          &CmdOpClient::handle_reset,
          this));
      services_last_ts_ = commands_clock_.now();
    }
  }
}

} // namespace dua_ros_topic_tools
