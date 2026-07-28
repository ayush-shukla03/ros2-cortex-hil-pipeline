#ifndef HIL_PROTOCOL_H
#define HIL_PROTOCOL_H

#include <stdint.h>

#define PACKET_HEADER 0xAA

// Force the compiler to pack this struct byte-by-byte
#pragma pack(push, 1)
typedef struct {
    uint8_t header;       // Always 0xAA to verify packet start
    float linear_vel;     // Target forward speed (m/s)
    float angular_vel;    // Target turning speed (rad/s)
    uint16_t checksum;    // Basic verification
} HIL_Command;
#pragma pack(pop)

#endif // HIL_PROTOCOL_H