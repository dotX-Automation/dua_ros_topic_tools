/**
 * Cmd Op client node implementation.
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

CmdOpClient::CmdOpClient(const rclcpp::NodeOptions & node_options)
: NodeBase("cmd_op_client", node_options, true)
{
  dua_init_node();

  RCLCPP_INFO(get_logger(), "Node initialized");
}

CmdOpClient::~CmdOpClient()
{
  // Wait for pending operations to complete
  if (op_thread_.joinable()) {
    op_thread_.join();
  }
}

void CmdOpClient::init_cgroups()
{
  cmd_op_cgroup_ = dua_create_exclusive_cgroup();
}

void CmdOpClient::init_subscribers()
{
  // cmd_op
  cmd_op_sub_ = dua_create_subscription<String>(
    "/cmd_op",
    std::bind(
      &CmdOpClient::cmd_op_sub_clbk,
      this,
      std::placeholders::_1),
    dua_qos::Reliable::get_datum_qos(),
    cmd_op_cgroup_);
}

void CmdOpClient::init_service_clients()
{
  // kill
  if (!services_kill_name_.empty()) {
    kill_client_ = dua_create_service_client<Trigger>(services_kill_name_, wait_servers_);
  }

  // reset
  if (!services_reset_name_.empty()) {
    reset_client_ = dua_create_service_client<Trigger>(services_reset_name_, wait_servers_);
  }
}

void CmdOpClient::init_action_clients()
{
  // arm
  if (!actions_arm_name_.empty()) {
    arm_client_ = dua_create_action_client<Arm>(actions_arm_name_, nullptr, wait_servers_);
  }

  // disarm
  if (!actions_disarm_name_.empty()) {
    disarm_client_ = dua_create_action_client<Disarm>(actions_disarm_name_, nullptr, wait_servers_);
  }
}

} // namespace dua_ros_topic_tools

#include <rclcpp_components/register_node_macro.hpp>
RCLCPP_COMPONENTS_REGISTER_NODE(dua_ros_topic_tools::CmdOpClient)
