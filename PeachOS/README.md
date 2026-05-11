macOS quick setup (same environment used for grading)

This project is built and tested on macOS using Homebrew tools and QEMU.
Apple Silicon is fine (QEMU emulates i386).

dependencies

Homebrew
brew update
brew install nasm mtools qemu i686-elf-binutils i686-elf-gcc

make 
nasm 
mcopy - mtools command used to copy hello.txt and .elf programs into the disk image FAT region.
mformat - mtools command that can initialize/format FAT on the image if host mtools requires it.
Why needed: mcopy places user .elf files into the image so the shell can run them.
Why mformat: some macOS setups (including ours) need FAT initialized first for mcopy to work.
qemu-system-i386 - Emulator used to boot and run the 32-bit PeachOS image on macOS.
i686-elf-gcc - Cross C compiler targeting i686-elf, required for freestanding kernel/user C code.
i686-elf-ld - Cross linker used during final kernel linking stages.


Little todo-list program we built so it runs on top of PeachOS as a normal
user program (todo.elf), launched from the shell.

It's nothing fancy: add stuff, list it, remove it, save/load to disk.
The point of the project was less about the todo app itself and more
about wiring up a user program end-to-end - syscalls, the int 0x80 path.

commands inside the app

add <task>      add a new task
list            show what's there
remove <id>     drop a task by its id
help            print commands
exit            back to the shell

ids are just whatever the kernel handed out, they don't reset when you
remove things - that's intentional so you can't accidentally reuse one
mid-session.

There are also save/load syscalls wired up on the kernel side (with a
little XOR-obfuscated on-disk format), but we kept them out of the
interactive REPL on purpose. We ran into coding and implementation issues.
Basically a skill issue. 


building


./build.sh
or
make all

The build script calls into the Makefile, which compiles the kernel,
the stdlib, the shell, and todo, then assembles everything into
bin/os.bin. The todo.elf gets mcopy'd into the FAT region of the
image so the shell can find it at runtime.

If the build complains about mtools or nasm, install those first.
On Debian/Ubuntu:

sudo apt install nasm mtools qemu-system-x86 build-essential


running
at the same directory level as PeachOS:

qemu-system-i386 -hda /bin/os.bin


Once it boots into the shell prompt, just type:

todo.elf (if nothing happens at first, just type it again.)

and the app takes over. exit drops you back to the shell.

