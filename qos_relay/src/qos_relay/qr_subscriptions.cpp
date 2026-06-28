/**
 * QoS Relay node topic subscriptions.
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

void QoSRelay::sub_clbk(std::shared_ptr<rclcpp::SerializedMessage> msg)
{
  pub_->publish(*msg);
}

} // namespace dua_ros_topic_tools
