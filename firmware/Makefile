# Define the toolchain
CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size

# Define the target architecture (Cortex-M7 with hardware FPU)
MACH = cortex-m7
FLOAT_ABI = hard
FPU = fpv5-d16

# Compiler Flags
CFLAGS = -mcpu=$(MACH) -mthumb -mfloat-abi=$(FLOAT_ABI) -mfpu=$(FPU)
CFLAGS += -O0 -g3 -Wall
CFLAGS += -Iinclude  # Look for header files in the /include directory

# Linker Flags
LDFLAGS = -nostdlib -T linker.ld -Wl,-Map=firmware.map

# Source and Output files
SRC = src/main.c src/uart_driver.c src/control_loop.c src/startup.c
OBJ = $(SRC:.c=.o)
TARGET = firmware.elf

# Default build target
all: $(TARGET)

# Compile C files into Object files
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Link Object files into the final ELF binary
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^
	$(SIZE) $@

# Clean command to remove compiled artifacts
clean:
	rm -f $(OBJ) $(TARGET) firmware.map