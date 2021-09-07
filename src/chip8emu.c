#include <stdio.h>
#include <string.h>
#include <ncurses.h>
#include <stdlib.h>
#include <time.h>

#define MEM_SIZE 4096
#define START_ADDR 0x200

long int loadRom(char* rom_in, unsigned char* ram_out, long int start_addr, long int max_size);
void execute(char* rom_in, int disFlag);
void initializeFont(unsigned char* ram_out);

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
    // initialize registers
    unsigned char regXY[16];
    memset(regXY, 0, sizeof(regXY));
    unsigned short regI = 0;
    unsigned short pc = 0;
    unsigned char delayTimer = 0;
    unsigned char soundTimer = 0;
    unsigned short stack[16];
    memset(stack, 0, sizeof(stack));
    unsigned short stackPointer = 0;
    
    // initialize display
    char display[64][32];
    memset(display, 1, sizeof(display));
    if (!disFlag) initscr();
    if (!disFlag) curs_set(0);

    // initialize ram
    unsigned char ram[MEM_SIZE];
    memset(ram, 0, MEM_SIZE);
    
    // load rom
    long int rom_size = 0;
    rom_size = loadRom(rom_in, ram, START_ADDR, MEM_SIZE); 
    
    // initialize font
    initializeFont(ram);

    // fetch/decode/execute
    for (pc = START_ADDR; pc < rom_size+START_ADDR; pc+=2)
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
                            memset(display, 0, sizeof(display));
                            clear();
                        }
                        break;

                    case 0xEE:
                        if (disFlag) printf("RET");
                        else
                        {
                            pc = stack[stackPointer--] -2; // because auto increment
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
                    pc = valNNN - 2; // loop will auto increment by 2 so offset
                }
                break;
            case 0x2:
                if (disFlag) printf("CALL %03x", valNNN);
                else
                {
                    stack[++stackPointer] = pc + 2;
                    pc = valNNN - 2; // compensate auto increment
                }
                break;
            case 0x3:
                if (disFlag) printf("SE V%x, %02x", regX, valNN);
                else
                {
                    
                    //printf("%x == %x: %x\n", regXY[regX], valNN, regXY[regX]== valNN);
                    //getch();
                    if (regXY[regX] == valNN)
                        pc += 2;
                }
                break;
            case 0x4:
                if (disFlag) printf("SNE V%x, %02x", regX, valNN);
                else
                {
                    if (regXY[regX] != (unsigned char) valNN)
                        pc += 2;
                }
                break;
            case 0x5:
                if (disFlag) printf("SE V%x, V%x", regX, regY);
                else
                {
                    if (regXY[regX] == regXY[regY])
                        pc+=2;
                }
                break;
            case 0x6:
                if (disFlag) printf("LD V%x, %02x", regX, valNN);
                else
                {
                    regXY[regX] = valNN;
                }
                break;
            case 0x7:
                if (disFlag) printf("ADD V%x, %02x", regX, valNN);
                else
                {
                    regXY[regX] += valNN;
                }
                break;
            case 0x8:
                switch (valN)
                {
                    case 0x0:
                        if (disFlag) printf("LD V%x, V%x", regX, regY);
                        else
                        {
                            regXY[regX] = regXY[regY];
                        }
                        break;
                    case 0x1:
                        if (disFlag) printf("OR V%x, V%x", regX, regY);
                        else
                        {
                            regXY[regX] |= regXY[regY];
                        }
                        break;
                    case 0x2:
                        if (disFlag) printf("AND V%x, V%x", regX, regY);
                        else
                        {
                            regXY[regX] &= regXY[regY];
                        }
                        break;
                    case 0x3:
                        if (disFlag) printf("XOR V%x, V%x", regX, regY);
                        else
                        {
                            regXY[regX] ^= regXY[regY];
                        }
                        break;
                    case 0x4:
                        if (disFlag) printf("ADD V%x, V%x", regX, regY);
                        else
                        {
                            unsigned short temp = (unsigned short) regXY[regX] + (unsigned short) regXY[regY];
                            // check carry
                            regXY[0xF] = ((temp >> 8) > 0);

                            regXY[regX] = (unsigned char) (temp & 0xFF);
                        }
                        break;
                    case 0x5:
                        if (disFlag) printf("SUB V%x, V%x", regX, regY);
                        else
                        {
                            regXY[0xF] = (regXY[regX] > regXY[regY]);
                            regXY[regX] -= regXY[regY];
                        }
                        break;
                    case 0x6:
                        if (disFlag) printf("SHR V%x {, V%x}", regX, regY);
                        else
                        {
                            // implementing to ignore loading regY
                            
                            // check carry
                            regXY[0xF] = regXY[regX] & 0x01;
                            regXY[regX] >>= 1;
                        }
                        break;
                    case 0x7:
                        if (disFlag) printf("SUBN V%x, V%x", regX, regY);
                        else
                        {
                            regXY[0xF] = (regXY[regY] > regXY[regX]);
                            regXY[regX] = regXY[regY] - regXY[regX];
                        }
                        break;
                    case 0xE:
                        if (disFlag) printf("SHL V%x {, V%x}", regX, regY);
                        else
                        {
                            //implementing to ignore loading regY

                            // check carry
                            regXY[0xF] = (regXY[regX] & 0x80) >> 7;
                            regXY[regX] <<= 1;
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
                    if (regXY[regX] != regXY[regY])
                        pc += 2;
                }
                break;
            case 0xA:
                if (disFlag) printf("LD I, %03x", valNNN);
                else
                {
                    regI = valNNN;
                }
                break;
            case 0xB:
                if (disFlag) printf("JP V0, %03x", valNNN);
                else
                {
                    pc = valNNN + regXY[0] - 2; //compensate for autoinc
                }
                break;
            case 0xC:
                if (disFlag) printf("RND V%x, %02x", regX, valNN);
                else
                {
                    time_t t;
                    srand((unsigned)time(&t));
                    unsigned char random = (unsigned char)(rand() % 0xFF);
                    regXY[regX] = random & valNN;
                }
                break;
            case 0xD:
                if (disFlag) printf("DRW V%x, V%x, %x", regX, regY, valN);
                else
                {
                    // get x value from reg[regX] % 64
                    unsigned char x = regXY[regX] % 64;
                    // get y value from reg[regY] % 32
                    unsigned char y = regXY[regY] % 32;
                    // reg[VF]=0
                    regXY[0xF] = 0;
                    // for i in valN rows
                    for (int i = 0; i < valN; i++)
                    {
                        unsigned char byte = ram[regI+i];
                        // for bit in byte
                        for (int j = 0; j < 8; j++)
                        {
                            unsigned char bit = (byte >> (7-j)) & 0x01;
                            if (bit == 1 && display[x][y] == 1)
                            {
                                display[x][y] = 0;
                                regXY[0xF] = 1;
                            }
                            else if (bit == 1 && display[x][y] == 0)
                            {
                                display[x][y] = 1;
                            }
                            x++;
                            if (x > 64) break;
                        }
                        x = regXY[regX] % 64;
                        y++;
                        if (y > 32) break;
                    }
                    clear();
                    for (short row=0; row < 32; row++)
                    {
                        for (short col=0; col < 64; col++)
                        {
                            if (display[col][row])
                                mvaddch(row, col, ACS_CKBOARD);
                        }
                    }
                    refresh();
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
        else
        {
            // wait to emulate processing speed
            napms(2);
            //getch();
            //printf("\n");
        }
    }
    endwin();
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

void initializeFont(unsigned char* ram_out)
{
    unsigned char font[16][5] = {
        {0xF0, 0x90, 0x90, 0x90, 0xF0}, // 0
        {0x20, 0x60, 0x20, 0x20, 0x70}, // 1
        {0xF0, 0x10, 0xF0, 0x80, 0xF0}, // 2
        {0xF0, 0x10, 0xF0, 0x10, 0xF0}, // 3
        {0x90, 0x90, 0xF0, 0x10, 0x10}, // 4
        {0xF0, 0x80, 0xF0, 0x10, 0xF0}, // 5
        {0xF0, 0x80, 0xF0, 0x90, 0xF0}, // 6
        {0xF0, 0x10, 0x20, 0x40, 0x40}, // 7
        {0xF0, 0x90, 0xF0, 0x90, 0xF0}, // 8
        {0xF0, 0x90, 0xF0, 0x10, 0xF0}, // 9
        {0xF0, 0x90, 0xF0, 0x90, 0x90}, // A
        {0xE0, 0x90, 0xE0, 0x90, 0xE0}, // B
        {0xF0, 0x80, 0x80, 0x80, 0xF0}, // C
        {0xE0, 0x90, 0x90, 0x90, 0xE0}, // D
        {0xF0, 0x80, 0xF0, 0x80, 0xF0}, // E
        {0xF0, 0x80, 0xF0, 0x80, 0x80}  // F
    };
    // font from 0x50 to 0x9F
    unsigned short ram_index = 0x50;
    for (int i = 0; i < 16; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            ram_out[ram_index++] = font[i][j];
        }
    }    
}
