#include <stdio.h>
#include <string.h>

#define MEM_SIZE 4096
#define START_ADDR 0x200

long int loadRom(char* rom_in, unsigned char* ram_out, long int start_addr, long int max_size);
void execute(char* rom_in, int disFlag);

void main(int argc, char**argv)
{
    // process command line options
    // [-d] <rom_file>: Execute ROM, if -d is present then disassemble instead 
    if (argc == 3 && !strcmp(argv[1], "-d"))
    {
        // disassemble option
        execute(argv[2], 1);
    }
    else if (argc == 2)
    {
        // execute
        execute(argv[1], 0);
    }
    else
    {
         printf("Usage: %s [-d] <rom_file>: specify -d to disassemble instead of execute.\n", argv[0]);
    }
}

void execute(char* rom_in, int disFlag)
{
    // initialize ram
    unsigned char ram[MEM_SIZE];
    for (int i = 0; i < MEM_SIZE; i++)
        ram[i] = 0;	
    
    // load rom
    long int rom_size = 0;
    rom_size = loadRom(rom_in, ram, START_ADDR, MEM_SIZE); 
    
    // fetch/decode/execute
    for (int pc = START_ADDR; pc < rom_size+START_ADDR; pc=pc+2)
    {
        unsigned short inst = (((unsigned short)ram[pc]) << 8) | ((unsigned short)ram[pc+1]);

        unsigned short opcode = (inst&0xF000)>>12;
        unsigned short regX = (inst&0x0F00)>>8;
        unsigned short regY = (inst&0x00F0)>>4;
        unsigned short valN = (inst&0x000F);
        unsigned short valNN = (inst&0x00FF);
        unsigned short valNNN = (inst&0x0FFF);

        if (disFlag) printf("%4x [%04x]: ", pc, inst);
        switch (opcode)
        {
            case 0x0:
                switch (valNN)
                {
                    case 0xE0:
                        if (disFlag) printf("CLS");
                        else
                        {
                        }
                        break;

                    case 0xEE:
                        if (disFlag) printf("RET");
                        else
                        {
                        }
                        break;

                    default: // 0x0nnn
                        if (disFlag) printf("SYS %03x", valNNN);
                        // not implementing
                        break;
                }
                break;
            case 0x1:
                if (disFlag) printf("JP %03x", valNNN);
                else
                {
                }
                break;
            case 0x2:
                if (disFlag) printf("CALL %03x", valNNN);
                else
                {
                }
                break;
            case 0x3:
                if (disFlag) printf("SE V%x, %02x", regX, valNN);
                else
                {
                }
                break;
            case 0x4:
                if (disFlag) printf("SNE V%x, %02x", regX, valNN);
                else
                {
                }
                break;
            case 0x5:
                if (disFlag) printf("SE V%x, V%x", regX, regY);
                else
                {
                }
                break;
            case 0x6:
                if (disFlag) printf("LD V%x, %02x", regX, valNN);
                else
                {
                }
                break;
            case 0x7:
                if (disFlag) printf("ADD V%x, %02x", regX, valNN);
                else
                {
                }
                break;
            case 0x8:
                switch (valN)
                {
                    case 0x0:
                        if (disFlag) printf("LD V%x, V%x", regX, regY);
                        else
                        {
                        }
                        break;
                    case 0x1:
                        if (disFlag) printf("OR V%x, V%x", regX, regY);
                        else
                        {
                        }
                        break;
                    case 0x2:
                        if (disFlag) printf("AND V%x, V%x", regX, regY);
                        else
                        {
                        }
                        break;
                    case 0x3:
                        if (disFlag) printf("XOR V%x, V%x", regX, regY);
                        else
                        {
                        }
                        break;
                    case 0x4:
                        if (disFlag) printf("ADD V%x, V%x", regX, regY);
                        else
                        {
                        }
                        break;
                    case 0x5:
                        if (disFlag) printf("SUB V%x, V%x", regX, regY);
                        else
                        {
                        }
                        break;
                    case 0x6:
                        if (disFlag) printf("SHR V%x {, V%x}", regX, regY);
                        else
                        {
                        }
                        break;
                    case 0x7:
                        if (disFlag) printf("SUBN V%x, V%x", regX, regY);
                        else
                        {
                        }
                        break;
                    case 0xE:
                        if (disFlag) printf("SHL V%x {, V%x}", regX, regY);
                        else
                        {
                        }
                        break;
                    default:
                        if (disFlag) printf("not supported");
                }
                break;
            case 0x9:
                if (disFlag) printf("SNE V%x, V%x", regX, regY);
                else
                {
                }
                break;
            case 0xA:
                if (disFlag) printf("LD I, %03x", valNNN);
                else
                {
                }
                break;
            case 0xB:
                if (disFlag) printf("JP V0, %03x", valNNN);
                else
                {
                }
                break;
            case 0xC:
                if (disFlag) printf("RND V%x, %02x", regX, valNN);
                else
                {
                }
                break;
            case 0xD:
                if (disFlag) printf("DRW V%x, V%x, %x", regX, regY, valN);
                else
                {
                }
                break;
            case 0xE:
                switch (valNN)
                {
                    case 0x9E:
                        if (disFlag) printf("SKP V%x", regX);
                        else
                        {
                        }
                        break;
                    case 0xA1:
                        if (disFlag) printf("SKNP V%x", regX);
                        else
                        {
                        }
                        break;
                    default:
                        if (disFlag) printf("not supported");
                        else
                        {
                        }
                        break;
                }
                break;
            case 0xF:
                switch (valNN)
                {
                    case 0x07:
                        if (disFlag) printf("LD V%x, DT", regX);
                        else
                        {
                        }
                        break;
                    case 0x0A:
                        if (disFlag) printf("LD V%x, K", regX);
                        else
                        {
                        }
                        break;
                    case 0x15:
                        if (disFlag) printf("LD DT, V%x", regX);
                        else
                        {
                        }
                        break;
                    case 0x18:
                        if (disFlag) printf("LD ST, V%x", regX);
                        else
                        {
                        }
                        break;
                    case 0x1E:
                        if (disFlag) printf("ADD I, V%x", regX);
                        else
                        {
                        }
                        break;
                    case 0x29:
                        if (disFlag) printf("LD F, V%x", regX);
                        else
                        {
                        }
                        break;
                    case 0x33:
                        if (disFlag) printf("LD B, V%x", regX);
                        else
                        {
                        }
                        break;
                    case 0x55:
                        if (disFlag) printf("LD [I], V%x", regX);
                        else
                        {
                        }
                        break;
                    case 0x65:
                        if (disFlag) printf("LD V%x, [I]", regX);
                        else
                        {
                        }
                        break;
                    default:
                        if (disFlag) printf("not supported");
                        break;
                }
                break;
            default:
                if (disFlag) printf("not supported");
                break;
        }
        if (disFlag) printf("\n");
    }
}

long int loadRom(char* rom_in, unsigned char* ram_out, long int start_addr, long int max_size)
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
    if (rom_size > (max_size-start_addr))
    {
	    printf("ROM is larger than RAM, only reading first %d bytes", max_size);
	    rom_size = (max_size-start_addr);
    }

    //read in
    for (int i = start_addr; i < (rom_size+start_addr); i++)
    {
        fread(&ram_out[i], sizeof(unsigned char), 1, romPtr);
    }

    fclose(romPtr);

    return rom_size;
}
