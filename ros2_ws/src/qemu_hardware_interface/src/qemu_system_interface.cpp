#include "hardware_interface/system_interface.hpp"
#include "hardware_interface/types/hardware_interface_type_values.hpp"
#include "rclcpp/rclcpp.hpp"
#include "pluginlib/class_list_macros.hpp"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

// Ensure this matches the struct in firmware/include/hil_protocol.h exactly
#pragma pack(push, 1)
struct HIL_Command {
    uint8_t header;
    float linear_vel;
    float angular_vel;
    uint16_t checksum;
};
#pragma pack(pop)

namespace qemu_hardware_interface
{
class QemuSystemInterface : public hardware_interface::SystemInterface
{
private:
    int sock_fd_;
    struct sockaddr_in server_addr_;
    
    // Internal state tracking
    double hw_cmd_linear_vel_ = 0.0;
    double hw_cmd_angular_vel_ = 0.0;

public:
    hardware_interface::CallbackReturn on_init(const hardware_interface::HardwareInfo & info) override
    {
        if (hardware_interface::SystemInterface::on_init(info) != hardware_interface::CallbackReturn::SUCCESS) {
            return hardware_interface::CallbackReturn::ERROR;
        }
        return hardware_interface::CallbackReturn::SUCCESS;
    }

    hardware_interface::CallbackReturn on_activate(const rclcpp_lifecycle::State & /*previous_state*/) override
    {
        // Open the TCP socket to connect to QEMU
        sock_fd_ = socket(AF_INET, SOCK_STREAM, 0);
        if (sock_fd_ < 0) {
            RCLCPP_ERROR(rclcpp::get_logger("QemuSystemInterface"), "Failed to create socket");
            return hardware_interface::CallbackReturn::ERROR;
        }

        server_addr_.sin_family = AF_INET;
        server_addr_.sin_port = htons(8888);
        inet_pton(AF_INET, "127.0.0.1", &server_addr_.sin_addr);

        if (connect(sock_fd_, (struct sockaddr *)&server_addr_, sizeof(server_addr_)) < 0) {
            RCLCPP_ERROR(rclcpp::get_logger("QemuSystemInterface"), "Failed to connect to QEMU at 127.0.0.1:8888");
            return hardware_interface::CallbackReturn::ERROR;
        }

        RCLCPP_INFO(rclcpp::get_logger("QemuSystemInterface"), "Successfully connected to QEMU bare-metal emulator!");
        return hardware_interface::CallbackReturn::SUCCESS;
    }

    hardware_interface::CallbackReturn on_deactivate(const rclcpp_lifecycle::State & /*previous_state*/) override
    {
        close(sock_fd_);
        return hardware_interface::CallbackReturn::SUCCESS;
    }

    // Map ROS 2 command variables to the internal state
    std::vector<hardware_interface::CommandInterface> export_command_interfaces() override
    {
        std::vector<hardware_interface::CommandInterface> command_interfaces;
        command_interfaces.emplace_back(hardware_interface::CommandInterface("base", "linear_velocity", &hw_cmd_linear_vel_));
        command_interfaces.emplace_back(hardware_interface::CommandInterface("base", "angular_velocity", &hw_cmd_angular_vel_));
        return command_interfaces;
    }

    std::vector<hardware_interface::StateInterface> export_state_interfaces() override
    {
        // For a one-way command bridge, state interfaces can be empty or mock actual states
        return {};
    }

    hardware_interface::return_type read(const rclcpp::Time & /*time*/, const rclcpp::Duration & /*period*/) override
    {
        // In a full bidirectional HIL, you would read encoder ticks from the socket here
        return hardware_interface::return_type::OK;
    }

    hardware_interface::return_type write(const rclcpp::Time & /*time*/, const rclcpp::Duration & /*period*/) override
    {
        HIL_Command cmd;
        cmd.header = 0xAA;
        cmd.linear_vel = static_cast<float>(hw_cmd_linear_vel_);
        cmd.angular_vel = static_cast<float>(hw_cmd_angular_vel_);
        cmd.checksum = 0; // Simplified for now

        // Stream the packed struct directly to the ARM Cortex-M7 over TCP
        if (send(sock_fd_, &cmd, sizeof(HIL_Command), 0) < 0) {
            RCLCPP_ERROR(rclcpp::get_logger("QemuSystemInterface"), "Failed to send data to QEMU");
            return hardware_interface::return_type::ERROR;
        }

        return hardware_interface::return_type::OK;
    }
};
} // namespace qemu_hardware_interface

// Export the class as a pluginlib plugin so ROS 2 can load it dynamically
PLUGINLIB_EXPORT_CLASS(qemu_hardware_interface::QemuSystemInterface, hardware_interface::SystemInterface)