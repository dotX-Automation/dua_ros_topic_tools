# qos_relay

ROS 2 node that bridges two topics with independent, configurable QoS settings. It subscribes to an input topic and republishes the same serialized messages on an output topic, enabling communication between publishers and subscribers that would otherwise be incompatible due to mismatched QoS policies.

## Overview

In heterogeneous ROS 2 systems it is common to find publishers and subscribers whose QoS policies are incompatible — e.g., when communication has to happen over a low-bandwidth link where, to avoid congestion, `BEST_EFFORT` reliability must be used. Modifying either side is not always feasible. `qos_relay` inserts itself between them, absorbing the mismatch transparently.

- Uses `rclcpp::GenericSubscription` and `rclcpp::GenericPublisher`, so it works with **any message type** without recompilation. The message type is autodiscovered at startup by querying the existing publishers on the input topic.
- Forwards **serialized messages** directly — no deserialization or re-serialization — so there is no message-type dependency and the overhead is minimal.
- Input and output QoS policies (reliability and history depth) are fully independent and configurable at runtime via a YAML parameter file.

## Parameters

All parameters are **read-only** (set at startup; cannot be changed at runtime).

| Parameter | Type | Default | Description |
| --- | --- | --- | --- |
| `input.name` | `string` | `/input` | Fully qualified name of the input topic. Must be a valid ROS 2 FQN. |
| `input.qos.depth` | `int` | `10` | History depth of the input subscriber [0–1000]. |
| `input.qos.reliability` | `string` | `reliable` | QoS reliability of the input subscriber. Must be `reliable` or `best_effort`. |
| `input.wait` | `bool` | `false` | If `true`, the node waits (polling every second) until the input topic is being published. If `false`, an exception is thrown immediately when no publisher is found at startup. |
| `output.name` | `string` | `/output` | Fully qualified name of the output topic. Must be a valid ROS 2 FQN. |
| `output.qos.depth` | `int` | `10` | History depth of the output publisher [0–1000]. |
| `output.qos.reliability` | `string` | `reliable` | QoS reliability of the output publisher. Must be `reliable` or `best_effort`. |

Topic FQNs are validated at startup with `rmw_validate_full_topic_name`. Invalid names cause fatal errors. When more than one publisher is active on the input topic, a warning is logged and the type of the first publisher is used.

## Topics

| Direction | Default name | Type | QoS |
| --- | --- | --- | --- |
| Subscription | `/input` (`input.name`) | Any (autodiscovered) | Configured via `input.qos.*` |
| Publication | `/output` (`output.name`) | Same as input | Configured via `output.qos.*` |

## Usage

### Standalone application

```bash
ros2 run qos_relay qos_relay_app --ros-args --params-file /path/to/qos_relay.yaml
```

### Launch file

```bash
ros2 launch qos_relay qos_relay.launch.py namespace:=<ns> cf:=/path/to/qos_relay.yaml
```

The launch file accepts two arguments:

| Argument | Default | Description |
| --- | --- | --- |
| `namespace` | *(empty)* | ROS 2 namespace for the node. |
| `cf` | Package default config | Path to the YAML parameter file. |

### Example configuration

The following configuration bridges a best-effort sensor topic into a reliable topic with a smaller queue:

```yaml
/qos_relay:
  ros__parameters:
    input:
      name: /sensor/data_raw
      qos:
        depth: 10
        reliability: best_effort
      wait: true
    output:
      name: /sensor/data
      qos:
        depth: 1
        reliability: reliable
```

## Component registration

The node is registered as a composable component under the `dua_ros_topic_tools::QoSRelay` class name and can be loaded into any `rclcpp` component container:

```bash
ros2 component load /ComponentManager qos_relay dua_ros_topic_tools::QoSRelay
```

## Copyright and License

Copyright 2026 dotX Automation s.r.l.

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License.

You may obtain a copy of the License at <http://www.apache.org/licenses/LICENSE-2.0>.

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.

See the License for the specific language governing permissions and limitations under the License.
