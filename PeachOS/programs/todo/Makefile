FILES=./build/main.o ./build/todo.o ./build/syscall.o ./build/encrypt.o ./build/disk.o
INCLUDES= -I../stdlib/src
FLAGS= -g -ffreestanding -falign-jumps -falign-functions -falign-labels -falign-loops -fstrength-reduce -fomit-frame-pointer -finline-functions -Wno-unused-function -fno-builtin -Werror -Wno-unused-label -Wno-cpp -Wno-unused-parameter -nostdlib -nostartfiles -nodefaultlibs -Wall -O0 -Iinc

all: ${FILES}
	i686-elf-gcc -g -T ./linker.ld -o ./todo.elf -ffreestanding -O0 -nostdlib -fpic -g ${FILES} ../stdlib/stdlib.elf

./build/main.o: ./main.c
	mkdir -p ./build
	i686-elf-gcc ${INCLUDES} -I./ $(FLAGS) -std=gnu99 -c ./main.c -o ./build/main.o

./build/todo.o: ./todo.c
	mkdir -p ./build
	i686-elf-gcc ${INCLUDES} -I./ $(FLAGS) -std=gnu99 -c ./todo.c -o ./build/todo.o

./build/syscall.o: ./syscall.c
	mkdir -p ./build
	i686-elf-gcc ${INCLUDES} -I./ $(FLAGS) -std=gnu99 -c ./syscall.c -o ./build/syscall.o

./build/encrypt.o: ./encrypt.c
	mkdir -p ./build
	i686-elf-gcc ${INCLUDES} -I./ $(FLAGS) -std=gnu99 -c ./encrypt.c -o ./build/encrypt.o

./build/disk.o: ./disk.c
	mkdir -p ./build
	i686-elf-gcc ${INCLUDES} -I./ $(FLAGS) -std=gnu99 -c ./disk.c -o ./build/disk.o

clean:
	rm -rf ${FILES}
	rm -f ./todo.elf
