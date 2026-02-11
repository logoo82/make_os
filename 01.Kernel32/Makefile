all: Kernel32.bin

Kernel32.bin: Source/EntryPoint.s Source/Main.c Source/Pages.c Source/ModeSwitch.asm

	nasm -o EntryPoint.bin Source/EntryPoint.s

	nasm -f elf32 -o ModeSwitch.o Source/ModeSwitch.asm

	gcc -m32 -ffreestanding -fno-pic -c Source/Main.c -o Main.o

	gcc -m32 -ffreestanding -fno-pic -c Source/Pages.c -o Pages.o

	ld -m elf_i386 -Ttext 0x10200 -e Main --oformat binary -N -o Main.bin Main.o Pages.o ModeSwitch.o

	cat EntryPoint.bin Main.bin > Kernel32.bin

clean:
	rm -f *.bin
	rm -f *.o	