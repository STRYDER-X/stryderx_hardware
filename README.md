# stryderx_hardware

![ROS 2](https://img.shields.io/badge/ROS%202-Humble-blue)
[![License](https://img.shields.io/badge/license-GPLv3-blue.svg)](LICENSE)

`stryderx_hardware` is a standalone ROS 2 package for StryderX hardware-facing nodes. It wraps low-level hardware libraries and scripts behind ROS 2 executables for camera streaming, joystick teleoperation, drive throttle control, and steering control.

## Package Contents

| Path                          | Purpose                                                                    |
| :---------------------------- | :------------------------------------------------------------------------- |
| `src/camera_server.cpp`       | C++ camera node that publishes compressed images and luminosity data.      |
| `src/joystick_teleop.cpp`     | C++ joystick teleop node that converts `/joy` input into `/cmd_vel`.       |
| `scripts/drive_controller`    | Python node that maps `/cmd_vel` linear input to ESC servo commands.       |
| `scripts/steering_controller` | Python node that maps `/cmd_vel` angular input to steering servo commands. |
| `third_party/camera_driver`   | Camera driver submodule used by `camera_server`.                           |
| `cmake/`                      | CMake helpers for executables, linting, and Doxygen docs.                  |

## Dependencies

- ROS 2 Humble.
- `ament_cmake` and `ament_cmake_python`.
- `rclcpp` and `rclpy`.
- `cv_bridge`, OpenCV 4, `sensor_msgs`, `geometry_msgs`, `std_msgs`, and `std_srvs`.
- `doxygen` for optional API documentation.
- `Rosmaster_Lib` available in the Python environment for drive and steering control.https://github.com/STRYDER-X
- Robot hardware access when running hardware nodes:
  - Camera device for `camera_server`.
  - Rosmaster-compatible controller for the drive and steering nodes.
  - Joystick input publisher such as `joy/joy_node` for `joystick_teleop`.

## Build

Place this package in the `src/` directory of a ROS 2 workspace. Because this package uses a camera driver submodule, initialize submodules after cloning:

```bash
git submodule update --init --recursive
```

From the workspace root, build this package:

```bash
colcon build --packages-select stryderx_hardware --symlink-install
source install/setup.bash
```

## Executables

Run nodes directly with `ros2 run` after building and sourcing the workspace.

Camera server:

```bash
ros2 run stryderx_hardware camera_server
```

Joystick teleop:

```bash
ros2 run stryderx_hardware joystick_teleop
```

Drive controller:

```bash
ros2 run stryderx_hardware drive_controller
```

Steering controller:

```bash
ros2 run stryderx_hardware steering_controller
```

For full robot startup, use this package together with a bringup package that launches the required nodes and parameters.

## Nodes And Interfaces

### `camera_server`

Publishes camera frames and luminosity readings from the configured camera device.

Topics and services prefixed with `~` are private names and expand under the node name. For example, `~/camera/image/compressed` becomes `/camera_server/camera/image/compressed`.

| Publisher                   | Type                              | Description                                        |
| :-------------------------- | :-------------------------------- | :------------------------------------------------- |
| `~/camera/image/compressed` | `sensor_msgs/msg/CompressedImage` | JPEG-compressed camera stream.                     |
| `~/luminosity_value`        | `std_msgs/msg/Float32`            | Luminosity value extracted from the current frame. |

| Service             | Type                   | Description                               |
| :------------------ | :--------------------- | :---------------------------------------- |
| `~/start_streaming` | `std_srvs/srv/Trigger` | Starts or resumes camera streaming.       |
| `~/pause_streaming` | `std_srvs/srv/Trigger` | Pauses camera streaming.                  |
| `~/shutdown_server` | `std_srvs/srv/Trigger` | Stops the server and shuts down the node. |

| Parameter         | Default      | Description                                        |
| :---------------- | :----------- | :------------------------------------------------- |
| `camera_name`     | `USB CAMERA` | Descriptive camera name.                           |
| `camera_type`     | `USB`        | Camera type label.                                 |
| `device_index`    | `0`          | Camera device index.                               |
| `fps`             | `30`         | Target camera frames per second.                   |
| `timeout_seconds` | `5`          | Consecutive frame failure timeout before shutdown. |

### `joystick_teleop`

Subscribes to joystick messages and publishes normalized velocity commands.

| Interface  | Type                      | Description                  |
| :--------- | :------------------------ | :--------------------------- |
| `/joy`     | `sensor_msgs/msg/Joy`     | Joystick input subscription. |
| `/cmd_vel` | `geometry_msgs/msg/Twist` | Velocity command publisher.  |

| Parameter       | Default | Description                           |
| :-------------- | :------ | :------------------------------------ |
| `left_joystick` | `0`     | Axis used for steering input.         |
| `left_trigger`  | `2`     | Axis used for reverse throttle input. |
| `right_trigger` | `5`     | Axis used for forward throttle input. |

### `drive_controller`

Subscribes to `/cmd_vel` and maps `linear.x` to ESC servo commands on servo ID `2`.

| Interface  | Type                      | Description                    |
| :--------- | :------------------------ | :----------------------------- |
| `/cmd_vel` | `geometry_msgs/msg/Twist` | Velocity command subscription. |

| Parameter           | Default | Description                                |
| :------------------ | :------ | :----------------------------------------- |
| `esc_min_angle`     | `80`    | ESC angle for one throttle limit.          |
| `esc_neutral_angle` | `90`    | ESC neutral angle used during cleanup.     |
| `esc_max_angle`     | `100`   | ESC angle for the opposite throttle limit. |
| `clamp_esc_angle`   | `true`  | Declared clamp setting for ESC output.     |

### `steering_controller`

Subscribes to `/cmd_vel` and maps `angular.z` to steering servo commands on servo ID `1`.

| Interface  | Type                      | Description                    |
| :--------- | :------------------------ | :----------------------------- |
| `/cmd_vel` | `geometry_msgs/msg/Twist` | Velocity command subscription. |

| Parameter   | Default | Description                                          |
| :---------- | :------ | :--------------------------------------------------- |
| `min_angle` | `40`    | Steering angle for one angular input limit.          |
| `max_angle` | `100`   | Steering angle for the opposite angular input limit. |

## Documentation

Generate Doxygen documentation through the CMake target:

```bash
colcon build --packages-select stryderx_hardware --cmake-target docs
```

The generated HTML output is written to `build/stryderx_hardware/docs/html/` in the workspace.

## Development Checks

Run package tests and lint checks from the workspace root:

```bash
colcon test --packages-select stryderx_hardware
colcon test-result --verbose
```

## Maintainer

- Julian A. Rendon
- julianrendon514@gmail.com
