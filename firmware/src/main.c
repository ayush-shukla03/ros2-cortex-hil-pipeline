#include "uart_driver.h"
#include "hil_protocol.h"
#include <stdint.h>

// Declare the kinematics function
typedef struct { float left_wheel_vel; float right_wheel_vel; } WheelSpeeds;
extern WheelSpeeds calculate_kinematics(HIL_Command target_cmd);

int main() {
    uart_init();
    uart_send_string("MCU Booted. Waiting for ROS2 HIL struct...\n");

    HIL_Command incoming_cmd;
    uint8_t *cmd_ptr = (uint8_t *)&incoming_cmd;

    while(1) {
        // 1. Wait for the exact header byte to align the packet
        if (uart_read_byte() == PACKET_HEADER) {
            cmd_ptr[0] = PACKET_HEADER;
            
            // 2. Read the remaining bytes of the struct directly into memory
            for (int i = 1; i < sizeof(HIL_Command); i++) {
                cmd_ptr[i] = uart_read_byte();
            }

            // 3. (Optional) Verify checksum here

            // 4. Pass the parsed command to your kinematics engine
            WheelSpeeds target_speeds = calculate_kinematics(incoming_cmd);
            (void)target_speeds; // Suppress unused variable warning for now
            
            // 5. Output the results (In the real system, you would send this to PWM registers)
            // For now, we will just echo it back to prove the math works
            uart_send_string("Kinematics Computed!\n");
        }
    }
    return 0;
}

// Vector Table for Cortex-M7 boot sequence
__attribute__((section(".vectors"), used))
void *vector_table[] = {
    (void *)0x20010000,
    (void *)main       
};