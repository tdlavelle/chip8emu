#include <stdio.h>
#include <string.h>

#define MEM_SIZE 4096
long int loadRom(char* rom_in, unsigned char* ram_out);
void disassemble(char* rom_in, char* file_out);

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
    
    long int rom_size = 0;
    rom_size = loadRom(rom_in, ram); 
    // fetch/decode/execute

    for (int pc = 0; pc < rom_size; pc++)
    {
        unsigned short inst = (((unsigned short)ram[pc]) << 8) | ((unsigned short)ram[++pc]);

        switch (inst & 0xF000)
        {
            case 0x0000:
                switch (inst)
                {
                    case 0x00E0:
                        printf("%d: CLS\n", (pc/2));
                        break;

                    case 0x00EE:
                        printf("%d: RET\n", (pc/2));
                        break;

                    default: // 0x0nnn
                        printf("%d: SYS %x\n", (pc/2), (inst&0xFFF));
                        break;
                }
                break;
	    case 0x1000:
                printf("%d: JP %x\n", (pc/2), (inst&0xFFF));
                break;
            case 0x2000:
                printf("%d: CALL %x\n", (pc/2), (inst&0xFFF));
                break;


	    
            default:
                printf("%d: %hx\n", (pc/2), inst);
                break;
        }
    }
}

long int loadRom(char* rom_in, unsigned char* ram_out)
{
    // open file
    FILE *romPtr = fopen(rom_in, "rb");
    if (romPtr == NULL)
    {
        printf("invalid file: %s\n", rom_in);
    }

    // determine ROM size
    long int rom_size = 0;
    fseek(romPtr, 0, SEEK_END);
    rom_size = ftell(romPtr);
    rewind(romPtr);
    if (rom_size > MEM_SIZE)
    {
	    printf("ROM is larger than RAM, only reading first %d bytes", MEM_SIZE);
	    rom_size = MEM_SIZE;
    }

    //read in
    for (int i = 0; i < rom_size; i++)
    {
        fread(&ram_out[i], sizeof(unsigned char), 1, romPtr);
        //printf("%x: %x\n", i, ram_out[i]);
    }

    fclose(romPtr);

    return rom_size;
}
