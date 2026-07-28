#include "hil_protocol.h"

#define WHEEL_BASE_METERS 0.3f // 30cm distance between wheels

typedef struct {
    float left_wheel_vel;
    float right_wheel_vel;
} WheelSpeeds;

WheelSpeeds calculate_kinematics(HIL_Command target_cmd) {
    WheelSpeeds speeds;
    
    // Differential drive inverse kinematics calculation
    float angular_component = (target_cmd.angular_vel * WHEEL_BASE_METERS) / 2.0f;
    
    speeds.right_wheel_vel = target_cmd.linear_vel + angular_component;
    speeds.left_wheel_vel  = target_cmd.linear_vel - angular_component;
    
    return speeds;
}
