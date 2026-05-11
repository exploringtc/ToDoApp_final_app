CC = i686-elf-gcc

BUILD_DIR = ./build
TARGET = ./todo.elf
STDLIB = ../stdlib/stdlib.elf

SRCS = ./main.c ./todo.c
OBJS = $(BUILD_DIR)/main.o $(BUILD_DIR)/todo.o

INCLUDES = -I../stdlib/src -I./
CFLAGS = -g -ffreestanding -falign-jumps -falign-functions -falign-labels -falign-loops \
         -fstrength-reduce -fomit-frame-pointer -finline-functions -Wno-unused-function \
         -fno-builtin -Werror -Wno-unused-label -Wno-cpp -Wno-unused-parameter \
         -nostdlib -nostartfiles -nodefaultlibs -Wall -O0 -Iinc -std=gnu99
LDFLAGS = -g -T ./linker.ld -ffreestanding -O0 -nostdlib -fpic

all: $(TARGET)

$(TARGET): $(OBJS) $(STDLIB)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(STDLIB)

$(BUILD_DIR)/%.o: ./%.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)
	rm -f $(TARGET)

.PHONY: all clean

