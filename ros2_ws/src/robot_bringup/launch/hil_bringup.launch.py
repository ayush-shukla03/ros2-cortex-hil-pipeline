import os
from launch import LaunchDescription
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory
import xacro

def generate_launch_description():
    # Get the URDF path
    pkg_description = get_package_share_directory('robot_description')
    urdf_file = os.path.join(pkg_description, 'urdf', 'robot.urdf.xacro')
    
    # Process Xacro
    doc = xacro.process_file(urdf_file)
    robot_description = {'robot_description': doc.toxml()}

    # Controller Config Path
    controller_params_file = os.path.join(pkg_description, 'config', 'controllers.yaml')

    # 1. Robot State Publisher
    node_robot_state_publisher = Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        output='screen',
        parameters=[robot_description]
    )

    # 2. Controller Manager (Loads the QemuSystemInterface plugin)
    node_controller_manager = Node(
        package='controller_manager',
        executable='ros2_control_node',
        parameters=[robot_description, controller_params_file],
        output="screen",
    )

    # 3. Spawners for the controllers
    spawn_diff_cont = Node(
        package="controller_manager",
        executable="spawner",
        arguments=["diff_cont"],
        output="screen",
    )

    spawn_jsb = Node(
        package="controller_manager",
        executable="spawner",
        arguments=["joint_state_broadcaster"],
        output="screen",
    )

    return LaunchDescription([
        node_robot_state_publisher,
        node_controller_manager,
        spawn_jsb,
        spawn_diff_cont,
    ])