cmake_minimum_required(VERSION 3.8)
project(qemu_hardware_interface)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Find dependencies
find_package(ament_cmake REQUIRED)
find_package(hardware_interface REQUIRED)
find_package(rclcpp REQUIRED)
find_package(pluginlib REQUIRED)

# Add the shared library for the hardware interface plugin
add_library(qemu_hardware_interface SHARED
  src/qemu_hardware_interface.cpp
)

target_include_directories(qemu_hardware_interface PRIVATE include)
ament_target_dependencies(qemu_hardware_interface
  hardware_interface
  rclcpp
  pluginlib
)

# Export the plugin XML for pluginlib discovery
pluginlib_export_plugin_description_file(hardware_interface qemu_hardware_interface.xml)

# Install rules
install(TARGETS 
  qemu_hardware_interface
  DESTINATION lib
)
install(FILES 
  qemu_hardware_interface.xml
  DESTINATION share/${PROJECT_NAME}
)

ament_package()