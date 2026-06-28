/**
 * QoS Relay node definition.
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

#include <chrono>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include <rclcpp/rclcpp.hpp>

#include <dua_node_cpp/dua_node.hpp>
#include <dua_qos_cpp/dua_qos.hpp>

namespace dua_ros_topic_tools
{

/**
 * Relays communication over an input/output topic pair with configurable QoS settings.
 */
class QoSRelay : public dua_node::NodeBase
{
public:
  /**
   * @brief Constructor.
   *
   * @param node_options Node options.
   */
  explicit QoSRelay(const rclcpp::NodeOptions & node_options = rclcpp::NodeOptions());

  /**
   * @brief Destructor.
   */
  virtual ~QoSRelay();

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
   *
   * @throws std::runtime_error if subscriber initialization fails.
   */
  void init_subscribers() override;

  /**
   * @brief Initializes publishers.
   */
  void init_publishers() override;

  /* Topic subscription callback groups. */
  rclcpp::CallbackGroup::SharedPtr sub_cgroup_;

  /* Topic subscriptions. */
  rclcpp::GenericSubscription::SharedPtr sub_;

  /* Topic subscription callbacks. */
  /**
   * @brief Republishes a serialized message.
   *
   * @param msg Message to republish.
   */
  void sub_clbk(std::shared_ptr<rclcpp::SerializedMessage> msg);

  /* Topic publishers. */
  rclcpp::GenericPublisher::SharedPtr pub_;

  /* Internal state variables. */
  std::string topic_type_;

  /* Node parameters. */
  bool input_wait_ = false;

  /* Utility routines. */
  /**
   * @brief Validates QoS reliability parameters.
   *
   * @param p Parameter to change.
   * @return True if parameter is valid, false otherwise.
   */
  bool validate_qos_reliability(const rclcpp::Parameter & p);

  /**
   * @brief Validates topic fully qualified names.
   *
   * @param p Parameter to change.
   * @return True if parameter is valid, false otherwise.
   */
  bool validate_topic_fqn(const rclcpp::Parameter & p);

  /* Constants. */
  static constexpr std::string_view QOS_RELIABILITY_STR_RELIABLE    = "reliable";
  static constexpr std::string_view QOS_RELIABILITY_STR_BEST_EFFORT = "best_effort";
};

} // namespace dua_ros_topic_tools
