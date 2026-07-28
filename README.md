# ROS 2 Cortex-M7 HIL Pipeline

This repository contains a Hardware-in-the-Loop (HIL) simulation pipeline that connects a ROS 2 system to an ARM Cortex-M7 emulator (QEMU). It utilizes a custom `ros2_control` hardware interface to stream differential drive kinematics down to bare-metal embedded firmware over a TCP socket.

## Features

* **Custom `ros2_control` Plugin:** Implements a `SystemInterface` (`qemu_hardware_interface`) that dynamically maps to URDF joint states and streams control payloads.
* **TCP Socket Communication:** Packs command data into a structured C/C++ struct (with a `0xAA` header byte) and streams it to QEMU over a local TCP port (8888).
* **Differential Drive Integration:** Fully compatible with the standard ROS 2 `diff_drive_controller` and `joint_state_broadcaster`.
* **Real-Time Safe Debugging:** Includes non-blocking modulo-counter logging to monitor socket payloads without relying on standard ROS clock overhead within the control loop.

## Prerequisites

* **OS:** Ubuntu 22.04 (Recommended)
* **ROS 2:** Humble Hawksbill
* **Dependencies:**
  * `ros2_control`
  * `ros2_controllers` (specifically `diff_drive_controller` and `joint_state_broadcaster`)
  * `xacro`
* **Emulator:** QEMU (`qemu-system-arm`)

## Installation

1. Clone this repository into your home directory (or preferred projects folder):
   ```bash
   cd ~
   git clone https://github.com/ayush-shukla03/ros2-cortex-hil-pipeline.git ros2-cortex-hil-pipeline
   ```

2. Navigate into the provided ROS 2 workspace and install dependencies:

    ```bash
    cd ~/ros2-cortex-hil-pipeline/ros2_ws
    rosdep install --from-paths src --ignore-src -r -y
    ```

3. Build and source the ROS 2 workspace:
    ```bash
    colcon build --symlink-install
    source install/setup.bash
    ```

---

## Usage

1. Launch the QEMU Simulator
Open a terminal and run the provided script to start the ARM Cortex-M7 emulator. It will wait for an incoming TCP connection on port 8888.

    ```bash
    cd ~/ros2-cortex-hil-pipeline
    ./scripts/launch_qemu.sh
    ```

2. Launch the ROS 2 Control Node
In a new terminal, navigate to the workspace, source it, and launch the hardware interface:

    ```bash
    cd ~/ros2-cortex-hil-pipeline/ros2_ws
    source install/setup.bash
    ros2 launch robot_bringup hil_bringup_launch.py
    ```
