# cmd_op_client

ROS 2 node that translates operational commands received over a topic into action and service calls. It subscribes to a string command topic and dispatches actions and service requests to the appropriate servers, acting as a command bridge between a high-level command publisher and the hardware-facing servers.

## Overview

In UxV systems it is common to have a single command channel — e.g., fed by a joystick node or a mission manager — that needs to trigger different hardware operations. Wiring the command publisher directly to every action or service server is possible but may be unfeasible over constrained communication links. `cmd_op_client` inserts itself in between, mapping well-known string tokens to the corresponding RPC calls.

- Supports built-in commands like `arm`, `disarm`, `kill`, and `reset`.
- Each client (action or service) is **optional**: leaving its name parameter empty disables that client entirely, so the node can be deployed with any subset of the four commands active.
- Actions and services each have an **independent, runtime-configurable cooldown** that prevents a burst of repeated messages from triggering the same operation multiple times.
- At most **one operation runs at a time**: a command received while a previous one is still executing is silently dropped.
- Worker threads handle blocking RPC calls off the subscription callback, keeping the executor responsive.

## Parameters

Parameters marked **read-only** are set at startup and cannot be changed at runtime.

| Parameter | Type | Default | Read-only | Description |
| --- | --- | --- | --- | --- |
| `actions.arm.name` | `string` | `""` | yes | Fully qualified name of the `Arm` action server. Set to empty string to disable. |
| `actions.cooldown` | `int` | `1000` | no | Minimum time between consecutive action commands [ms, 1–10000]. |
| `actions.disarm.name` | `string` | `""` | yes | Fully qualified name of the `Disarm` action server. Set to empty string to disable. |
| `services.cooldown` | `int` | `1000` | no | Minimum time between consecutive service commands [ms, 1–10000]. |
| `services.kill.name` | `string` | `""` | yes | Fully qualified name of the kill-switch service. Set to empty string to disable. |
| `services.reset.name` | `string` | `""` | yes | Fully qualified name of the reset service. Set to empty string to disable. |
| `wait_servers` | `bool` | `false` | yes | If `true`, the node waits for all configured action and service servers to become available before completing initialization. |

## Topics

| Direction | Default name | Type | QoS |
| --- | --- | --- | --- |
| Subscription | `/cmd_op` | `std_msgs/msg/String` | Reliable by default, can be overridden if necessary |

## Commands

The following string values are recognized on the `/cmd_op` topic. Unrecognized strings are ignored.

| Command string | RPC type | Interface | Condition |
| --- | --- | --- | --- |
| `arm` | Action | `dua_hardware_interfaces/action/Arm` | `actions.arm.name` not empty, cooldown elapsed |
| `disarm` | Action | `dua_hardware_interfaces/action/Disarm` | `actions.disarm.name` not empty, cooldown elapsed |
| `kill` | Service | `std_srvs/srv/Trigger` | `services.kill.name` not empty, cooldown elapsed |
| `reset` | Service | `std_srvs/srv/Trigger` | `services.reset.name` not empty, cooldown elapsed |

The actions and services cooldown timers are tracked independently: an `arm` command and a `kill` command can be dispatched simultaneously, but two consecutive `arm` commands within `actions.cooldown` milliseconds will result in the second being dropped.

## Usage

### Standalone application

```bash
ros2 run cmd_op_client cmd_op_client_app --ros-args --params-file /path/to/cmd_op_client.yaml
```

### Launch file

```bash
ros2 launch cmd_op_client cmd_op_client.launch.py namespace:=<ns> cf:=/path/to/cmd_op_client.yaml
```

The launch file accepts two arguments:

| Argument | Default | Description |
| --- | --- | --- |
| `namespace` | *(empty)* | ROS 2 namespace for the node. |
| `cf` | Package default config | Path to the YAML parameter file. |

### Example configuration

The following configuration enables all four commands with a 2-second cooldown for actions and a 500 ms cooldown for services:

```yaml
/cmd_op_client:
  ros__parameters:
    actions:
      arm:
        name: /vehicle/arm
      disarm:
        name: /vehicle/disarm
      cooldown: 2000
    services:
      kill:
        name: /vehicle/kill
      reset:
        name: /vehicle/reset
      cooldown: 500
    wait_servers: true
```

## Component registration

The node is registered as a composable component under the `dua_ros_topic_tools::CmdOpClient` class name and can be loaded into any `rclcpp` component container:

```bash
ros2 component load /ComponentManager cmd_op_client dua_ros_topic_tools::CmdOpClient
```

## Copyright and License

Copyright 2026 dotX Automation s.r.l.

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License.

You may obtain a copy of the License at <http://www.apache.org/licenses/LICENSE-2.0>.

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.

See the License for the specific language governing permissions and limitations under the License.
