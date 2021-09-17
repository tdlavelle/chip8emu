#! /bin/sh
BIN_DIR=$HOME/repo/chip8emu/bin/
SRC_DIR=$HOME/repo/chip8emu/chip8emu/src
EXE_NAME=chip8emu

gcc -std=c99 -o $BIN_DIR$EXE_NAME $SRC_DIR/chip8emu.c -lncurses
