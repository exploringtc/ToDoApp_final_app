# Object files that make up the todo userspace program.
FILES=./build/main.o ./build/todo.o ./build/syscall.o ./build/encrypt.o ./build/disk.o
# Include path for shared userspace stdlib headers.
INCLUDES= -I../stdlib/src
# Freestanding build flags compatible with PeachOS userspace environment.
FLAGS= -g -ffreestanding -falign-jumps -falign-functions -falign-labels -falign-loops -fstrength-reduce -fomit-frame-pointer -finline-functions -Wno-unused-function -fno-builtin -Werror -Wno-unused-label -Wno-cpp -Wno-unused-parameter -nostdlib -nostartfiles -nodefaultlibs -Wall -O0 -Iinc

# Link todo objects with the shared stdlib image to produce final ELF.
all: ${FILES}
	i686-elf-gcc -g -T ./linker.ld -o ./todo.elf -ffreestanding -O0 -nostdlib -fpic -g ${FILES} ../stdlib/stdlib.elf

# Compile program entrypoint.
./build/main.o: ./main.c
	mkdir -p ./build
	i686-elf-gcc ${INCLUDES} -I./ $(FLAGS) -std=gnu99 -c ./main.c -o ./build/main.o

# Compile command parsing and REPL logic.
./build/todo.o: ./todo.c
	mkdir -p ./build
	i686-elf-gcc ${INCLUDES} -I./ $(FLAGS) -std=gnu99 -c ./todo.c -o ./build/todo.o

# Compile syscall bridge wrappers.
./build/syscall.o: ./syscall.c
	mkdir -p ./build
	i686-elf-gcc ${INCLUDES} -I./ $(FLAGS) -std=gnu99 -c ./syscall.c -o ./build/syscall.o

# Compile XOR encryption helper.
./build/encrypt.o: ./encrypt.c
	mkdir -p ./build
	i686-elf-gcc ${INCLUDES} -I./ $(FLAGS) -std=gnu99 -c ./encrypt.c -o ./build/encrypt.o

# Compile disk save/load command wrappers.
./build/disk.o: ./disk.c
	mkdir -p ./build
	i686-elf-gcc ${INCLUDES} -I./ $(FLAGS) -std=gnu99 -c ./disk.c -o ./build/disk.o

# Remove generated objects and the final executable image.
clean:
	rm -rf ${FILES}
	rm -f ./todo.elf
