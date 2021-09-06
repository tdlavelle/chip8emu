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

    for (int pc = 0x200; pc < rom_size+0x200; pc++)
    {
        unsigned short inst = (((unsigned short)ram[pc]) << 8) | ((unsigned short)ram[++pc]);

        unsigned short opcode = (inst&0xF000)>>12;
        unsigned short regX = (inst&0x0F00)>>8;
        unsigned short regY = (inst&0x00F0)>>4;
        unsigned short valN = (inst&0x000F);
        unsigned short valNN = (inst&0x00FF);
        unsigned short valNNN = (inst&0x0FFF);

        printf("%4x [%04x]: ", (pc-1), inst);
        switch (opcode)
        {
            case 0x0:
                switch (valNN)
                {
                    case 0xE0:
                        printf("CLS");
                        break;

                    case 0xEE:
                        printf("RET");
                        break;

                    default: // 0x0nnn
                        printf("SYS %03x", valNNN);
                        break;
                }
                break;
            case 0x1:
                printf("JP %03x", valNNN);
                break;
            case 0x2:
                printf("CALL %03x", valNNN);
                break;
            case 0x3:
                printf("SE V%x, %02x", regX, valNN);
                break;
            case 0x4:
                printf("SNE V%x, %02x", regX, valNN);
                break;
            case 0x5:
                printf("SE V%x, V%x", regX, regY);
                break;
            case 0x6:
                printf("LD V%x, %02x", regX, valNN);
                break;
            case 0x7:
                printf("ADD V%x, %02x", regX, valNN);
                break;
            case 0x8:
                switch (valN)
                {
                    case 0x0:
                        printf("LD V%x, V%x", regX, regY);
                        break;
                    case 0x1:
                        printf("OR V%x, V%x", regX, regY);
                        break;
                    case 0x2:
                        printf("AND V%x, V%x", regX, regY);
                        break;
                    case 0x3:
                        printf("XOR V%x, V%x", regX, regY);
                        break;
                    case 0x4:
                        printf("ADD V%x, V%x", regX, regY);
                        break;
                    case 0x5:
                        printf("SUB V%x, V%x", regX, regY);
                        break;
                    case 0x6:
                        printf("SHR V%x {, V%x}", regX, regY);
                        break;
                    case 0x7:
                        printf("SUBN V%x, V%x", regX, regY);
                        break;
                    case 0xE:
                        printf("SHL V%x {, V%x}", regX, regY);
                        break;
                    default:
                        printf("not supported");
                }
                break;
            case 0x9:
                printf("SNE V%x, V%x", regX, regY);
                break;
            case 0xA:
                printf("LD I, %03x", valNNN);
                break;
            case 0xB:
                printf("JP V0, %03x", valNNN);
                break;
            case 0xC:
                printf("RND V%x, %02x", regX, valNN);
                break;
            case 0xD:
                printf("DRW V%x, V%x, %x", regX, regY, valN);
                break;
            case 0xE:
                switch (valNN)
                {
                    case 0x9E:
                        printf("SKP V%x", regX);
                        break;
                    case 0xA1:
                        printf("SKNP V%x", regX);
                        break;
                    default:
                        printf("not supported");
                        break;
                }
                break;
            case 0xF:
                switch (valNN)
                {
                    case 0x07:
                        printf("LD V%x, DT", regX);
                        break;
                    case 0x0A:
                        printf("LD V%x, K", regX);
                        break;
                    case 0x15:
                        printf("LD DT, V%x", regX);
                        break;
                    case 0x18:
                        printf("LD ST, V%x", regX);
                        break;
                    case 0x1E:
                        printf("ADD I, V%x", regX);
                        break;
                    case 0x29:
                        printf("LD F, V%x", regX);
                        break;
                    case 0x33:
                        printf("LD B, V%x", regX);
                        break;
                    case 0x55:
                        printf("LD [I], V%x", regX);
                        break;
                    case 0x65:
                        printf("LD V%x, [I]", regX);
                        break;
                }
                break;
            default:
                printf("not supported");
                break;
        }
        printf("\n");
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
    if (rom_size > (MEM_SIZE-0x200))
    {
	    printf("ROM is larger than RAM, only reading first %d bytes", MEM_SIZE);
	    rom_size = (MEM_SIZE-0x200);
    }

    //read in
    for (int i = 0x200; i < (rom_size+0x200); i++)
    {
        fread(&ram_out[i], sizeof(unsigned char), 1, romPtr);
        //printf("%x: %x\n", i, ram_out[i]);
    }

    fclose(romPtr);

    return rom_size;
}
