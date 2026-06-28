/**
 * QoS Relay node implementation.
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

#include <qos_relay/qos_relay.hpp>

namespace dua_ros_topic_tools
{

QoSRelay::QoSRelay(const rclcpp::NodeOptions & node_options)
: NodeBase("qos_relay", node_options, true)
{
  dua_init_node();

  RCLCPP_INFO(get_logger(), "Node initialized");
}

QoSRelay::~QoSRelay()
{}

void QoSRelay::init_cgroups()
{
  sub_cgroup_ = dua_create_exclusive_cgroup();
}

void QoSRelay::init_subscribers()
{
  // Get input topic name and type (by use-case convention: the first one (and supposedly only one) in the info vector)
  std::string input_name = get_parameter("input.name").as_string();
  while (true) {
    std::vector<rclcpp::TopicEndpointInfo> input_info = get_publishers_info_by_topic(input_name);
    if (input_info.size() == 0) {
      if (input_wait_) {
        RCLCPP_WARN(get_logger(), "Input topic not published yet...");
        std::this_thread::sleep_for(std::chrono::seconds(1));
      } else {
        throw std::runtime_error("Failed to get input topic type, is it being published?");
      }
    } else {
      if (input_info.size() > 1)
        RCLCPP_WARN(get_logger(), "More than one publisher detected on input topic");
      topic_type_ = input_info[0].topic_type();
      break;
    }
  }

  // Get input topic QoS parameters
  int64_t qos_depth = get_parameter("input.qos.depth").as_int();
  std::string qos_rel_str = get_parameter("input.qos.reliability").as_string();

  // Configure input topic subscriber callback group
  rclcpp::SubscriptionOptions input_sub_opts;
  input_sub_opts.callback_group = sub_cgroup_;

  // Subscribe to input topic
  sub_ = create_generic_subscription(
    input_name,
    topic_type_,
    qos_rel_str == QOS_RELIABILITY_STR_RELIABLE ?
      dua_qos::Reliable::get_datum_qos(qos_depth) : dua_qos::BestEffort::get_datum_qos(qos_depth),
    std::bind(
      &QoSRelay::sub_clbk,
      this,
      std::placeholders::_1),
    input_sub_opts);
  RCLCPP_INFO(get_logger(), "[TOPIC SUB] '%s'", sub_->get_topic_name());
}

void QoSRelay::init_publishers()
{
  // Get output topic name (type is already initialized by effect of NodeBase initialization order)
  std::string output_name = get_parameter("output.name").as_string();

  // Get output topic QoS parameters
  int64_t qos_depth = get_parameter("output.qos.depth").as_int();
  std::string qos_rel_str = get_parameter("output.qos.reliability").as_string();

  // Activate output topic publisher
  pub_ = create_generic_publisher(
    output_name,
    topic_type_,
    qos_rel_str == QOS_RELIABILITY_STR_RELIABLE ?
      dua_qos::Reliable::get_datum_qos(qos_depth) : dua_qos::BestEffort::get_datum_qos(qos_depth));
  RCLCPP_INFO(get_logger(), "[TOPIC PUB] '%s'", pub_->get_topic_name());
}

} // namespace dua_ros_topic_tools

#include <rclcpp_components/register_node_macro.hpp>
RCLCPP_COMPONENTS_REGISTER_NODE(dua_ros_topic_tools::QoSRelay);
