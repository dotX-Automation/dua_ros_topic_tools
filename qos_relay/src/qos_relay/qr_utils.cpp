/**
 * QoS Relay node utility routines.
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

#include <rmw/validate_full_topic_name.h>

namespace dua_ros_topic_tools
{

bool QoSRelay::validate_qos_reliability(const rclcpp::Parameter & p)
{
  std::string p_str = p.as_string();

  if (p_str != QOS_RELIABILITY_STR_RELIABLE && p_str != QOS_RELIABILITY_STR_BEST_EFFORT) {
    RCLCPP_FATAL(get_logger(), "Invalid QoS reliability setting: '%s'", p_str.c_str());
    return false;
  }
  return true;
}

bool QoSRelay::validate_topic_fqn(const rclcpp::Parameter & p)
{
  std::string topic_fqn = p.as_string();

  int validation_result = RMW_TOPIC_VALID;
  rmw_ret_t ret = rmw_validate_full_topic_name(topic_fqn.c_str(), &validation_result, nullptr);

  if (ret != RMW_RET_OK) {
    RCLCPP_FATAL(get_logger(), "Failed to validate topic FQN (%d)", ret);
    return false;
  }

  if (validation_result != RMW_TOPIC_VALID) {
    const char * reason = rmw_full_topic_name_validation_result_string(validation_result);
    RCLCPP_FATAL(get_logger(), "Invalid topic FQN '%s': '%s'", topic_fqn.c_str(), reason);
    return false;
  }

  return true;
}

} // namespace dua_ros_topic_tools
