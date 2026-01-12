all: Kernel32.bin

Kernel32.bin: Source/EntryPoint.s Source/Main.c

	nasm -o EntryPoint.bin Source/EntryPoint.s

	gcc -m32 -ffreestanding -fno-pic -c Source/Main.c -o Main.o

	ld -m elf_i386 -Ttext 0x10200 -e Main --oformat binary -N -o Main.bin Main.o

	cat EntryPoint.bin Main.bin > Kernel32.bin

clean:
	rm -f *.bin
	rm -f *.o