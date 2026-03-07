# stryderx_hardware

![ROS 2](https://img.shields.io/badge/ROS%202-Humble-blue)
[![License](https://img.shields.io/badge/license-GNU%20GPL-blue.svg)](LICENSE)

This is a ROS 2 robotics hardware package built with a strict Hardware Abstraction Layer (HAL). It wraps standalone C++/Python drivers into standardized ROS 2 nodes, making the robotics stack modular and easy to port across different platforms.

Think of this package as the "glue" layer. It doesn't talk to the hardware directly; instead, it takes our independent drivers and plugs them into the ROS 2 ecosystem, handling their lifecycle and communication.

## System Architecture

I used a two-part system to keep our hardware logic separated from our middleware

1. **Low-Level Drivers**: Pure C++/Python libraries with zero middleware dependencies.
2. **High-Level Wrappers**: ROS 2 nodes that bridge the libraries into the ROS ecosystem.

## Key Features

At the moment the following features have been implemented.

- **Camera Server**: Image streaming for US.

> [!NOTE]
> This project is currently a **Work in Progress**.

## Getting Started

### Prerequisites

- **ROS 2:** Humble (LTS).
- **Tools**: `colcon`, `cmake`, and a C++17 compliant compiler.
- **Doxygen**: Documentation for C++ code. ***(Optional)***

## Quick Start

You will need to build this package inside a standard ROS 2 workspace:

1. Head to your workspace and clone the repository:

   ```bash
   cd ~/<your_workspace>/src
   ```

   ```bash
   git clone --recursive git@github.com:STRYDER-X/stryderx_hardware.git
   ```

2. Build the package

   ```bash
   cd .. && colcon build --packages-select stryderx_hardware --symlink-install
   ```

### Nodes and Interfaces

#### `camera_server_node`

Main node for handling visual data and environmental lighting.

> [!TIP]
> Topics prefixed with `~` are private and will expand to include the node name (e.g.,
>  `/camera_server_node/camera/image/compressed`).

#### Publishers
| Topic | Type | Description |
| :--- | :--- | :--- |
| `~/camera/image/compressed` | `sensor_msgs/msg/CompressedImage` | The JPEC compressed video stream. |
| `~/luminosity_value` | `std_msgs/msg/Float32` | Current light levels for environmental awareness. |

#### Services
| Service | Type | Description |
| :--- | :--- | :--- |
| `~/start_streaming` | `std_srvs/srv/Trigger` | Connects to the hardware and kicks off the stream. Also used to resume streaming. |
| `~/pause_streaming` | `std_srvs/srv/Trigger` | Pauses the stream. |
| `~/shutdown_server` | `std_srvs/srv/Trigger` | Safely releases the hardware and stops the node. |

## Documentation

I used **Doxygen** to genearte API reference for the C++ codebase. Since I use a CMake-based documentation flow, you should generate the docs through the build system rather than running Doxygen manually

To generate the documentation:

```bash
# Build the specific `docs` target
coclon build --packages-select stryderx_hardware --cmake-target docs
```

Once the build finishes, your documentation will be available in the `build/stryderx_hardware/docs/html/` directory. Open `index.html` in your browswer to view it.

## Contributing

Contribution are welcomed! To keep the code base somewhat clean and compatible across different ROS versions, please adhere to the following guidelines.

### Git & Branching Strategy

To keep things organized and ensure long-term compatabilitiy, I used **Distro Silos** for ROS 2 versions
(like hunble, jazzy, iron). This helps isolate changes for specific ROS 2 distributions.

### Branch Naming Convention:

Branches are named with the following convention: `type/distro/description-issue#`

- **Types**: `feat/`, `fix`, `refactor`, `doc`, `test`.
- **Example**: `feat/humble/camera-server-logic-102`.

### Quality Control

- **Workspace Protection**: Automated via `pre-commit` hooks.
- **Drivers First**: Any new hardware features should be added to underlying agnostic library before
wrapping them here.
- **Linting**: I used `uncrustify` for C++ formatting. Run `ament_uncrustify` before opening a pull request.

## Author & Maintainer

- **Julian A. Rendon**
- Email: julianrendon514@gmail.com
