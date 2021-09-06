#include <stdio.h>
#include <string.h>

#define MEM_SIZE 4096

void main(int argc, char**argv)
{
	// process command line options
	// -d <rom_file> <output_file>: disassemble <rom_file> to <output_file>
	// -e <rom_file>: execute <rom_file>
	if (argc > 1)
	{
		printf("argv[1] = %s\nargc = %d\n", argv[1], argc);

		// disassemble option
		if (!strcmp(argv[1], "-d") && argc == 4)
		{
			printf("disassembling %s, writing to %s\n", argv[2], argv[3]);
			disassemble(argv[2], argv[3]);
		}
		else
		{
			printf("Incorrect arguments: -d <rom_file> <output_file>: disassemble <rom_file> to <output_file>\n");
		}
	}
	else
	{
		printf("hello, world\n");
	}
}

void disassemble(char* rom_in, char* file_out)
{
	// read in rom
	unsigned char ram[MEM_SIZE]; // should probably dynamically allocate
	for (int i = 0; i < MEM_SIZE; i++)
	{
		ram[i] = 0;
	}	
	loadRom(rom_in, ram); 
	// fetch/decode/execute
}

void loadRom(char* rom_in, unsigned char* ram_out)
{
	// open file
	FILE *romPtr = fopen(rom_in, "rb");
	if (romPtr == NULL)
	{
		printf("invalid file: %s\n", rom_in);
	}

	//read in
	for (int i = 0; i < MEM_SIZE; i++)
	{
		fread(&ram_out[i], sizeof(unsigned char), 1, romPtr);
		printf("%x: %x\n", i, ram_out[i]);
	}

	fclose(romPtr);
}
