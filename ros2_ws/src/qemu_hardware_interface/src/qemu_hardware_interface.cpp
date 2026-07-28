#include "hardware_interface/system_interface.hpp"
#include "hardware_interface/types/hardware_interface_type_values.hpp"
#include "rclcpp/rclcpp.hpp"
#include "pluginlib/class_list_macros.hpp"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <vector>

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
    
    // Internal state tracking for 2 wheels (left and right)
    std::vector<double> hw_commands_{0.0, 0.0};
    std::vector<double> hw_states_position_{0.0, 0.0};
    std::vector<double> hw_states_velocity_{0.0, 0.0};

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

    std::vector<hardware_interface::CommandInterface> export_command_interfaces() override
    {
        std::vector<hardware_interface::CommandInterface> command_interfaces;
        // Dynamically bind to the left and right wheel joints parsed from the URDF
        for (uint i = 0; i < info_.joints.size(); i++) {
            command_interfaces.emplace_back(hardware_interface::CommandInterface(
                info_.joints[i].name, hardware_interface::HW_IF_VELOCITY, &hw_commands_[i]));
        }
        return command_interfaces;
    }

    std::vector<hardware_interface::StateInterface> export_state_interfaces() override
    {
        std::vector<hardware_interface::StateInterface> state_interfaces;
        // Provide the mock states strictly demanded by the URDF
        for (uint i = 0; i < info_.joints.size(); i++) {
            state_interfaces.emplace_back(hardware_interface::StateInterface(
                info_.joints[i].name, hardware_interface::HW_IF_POSITION, &hw_states_position_[i]));
            state_interfaces.emplace_back(hardware_interface::StateInterface(
                info_.joints[i].name, hardware_interface::HW_IF_VELOCITY, &hw_states_velocity_[i]));
        }
        return state_interfaces;
    }

    hardware_interface::return_type read(const rclcpp::Time & /*time*/, const rclcpp::Duration & /*period*/) override
    {
        // Mock state updates so controllers don't fail (assuming perfect execution)
        for (uint i = 0; i < hw_commands_.size(); i++) {
            hw_states_velocity_[i] = hw_commands_[i];
            hw_states_position_[i] += hw_states_velocity_[i] * 0.01; // Assuming ~100Hz update rate
        }
        return hardware_interface::return_type::OK;
    }

    hardware_interface::return_type write(const rclcpp::Time & /*time*/, const rclcpp::Duration & /*period*/) override
    {
        // Extract wheel speeds (Index 0 = Left, Index 1 = Right based on URDF order)
        double w_l = hw_commands_[0];
        double w_r = hw_commands_[1];

        // Kinematics based on your URDF: radius = 0.1m, track width = 0.35m
        double wheel_radius = 0.1;
        double track_width = 0.35; 

        // Convert individual wheel velocities back to linear and angular velocities
        double linear_velocity = wheel_radius * (w_r + w_l) / 2.0;
        double angular_velocity = wheel_radius * (w_r - w_l) / track_width;

        HIL_Command cmd;
        cmd.header = 0xAA;
        cmd.linear_vel = static_cast<float>(linear_velocity);
        cmd.angular_vel = static_cast<float>(angular_velocity);
        cmd.checksum = 0;

        // Stream to the ARM Cortex-M7 over TCP
        if (send(sock_fd_, &cmd, sizeof(HIL_Command), MSG_NOSIGNAL) < 0) {
            RCLCPP_ERROR(rclcpp::get_logger("QemuSystemInterface"), "Failed to send data to QEMU");
            return hardware_interface::return_type::ERROR;
        }

        return hardware_interface::return_type::OK;
    }
};
} // namespace qemu_hardware_interface

PLUGINLIB_EXPORT_CLASS(
    qemu_hardware_interface::QemuSystemInterface, 
    hardware_interface::SystemInterface
)