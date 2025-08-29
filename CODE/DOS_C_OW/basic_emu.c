// Standard C includes
#include <stdio.h> // For printing
#include <stdlib.h> // For malloc

// Include Platform specific 
#include <dos.h>

// Project specific includes
#include "x86_emu.h"
#include "bios_emu.h"

// Module-specific includes
#include "basic_emu.h"

// Mimics BASIC Clear Screen (CLS) function
void _basic_cls(){
	_bios_set_video_mode(_bios_get_video_mode()); // Reset the screen to the current video mode
}

void _basic_print_newline(){
	printf("\r\n");
}

void _basic_print_char(char c){
	printf("%c",c);
}

// Mimics BASIC print with semicolon
void _basic_print(char* s){
	printf("%s",s);
}

// Mimics BASIC print without semicolon by appending CRLF
void _basic_println(char* s){
    _basic_print(s);
	_basic_print_newline();
}

void _basic_print_hex_8(unsigned char hex_byte){
	printf("%02X",hex_byte);
}

void _basic_print_hex_16(unsigned int hex_word){
	printf("%04X",hex_word);
}

// Mimics BASIC locate function (indexed at 1)
void _basic_locate(char row, char col){
	union REGPACK* reg_pack;
	reg_pack = malloc(sizeof(union REGPACK)); // Allocate memory
		
    reg_pack->h.ah = int_video_get_mode;
    intr(int_video,reg_pack); // Sets active page
	
    reg_pack->h.ah = int_video_set_cursor_pos;
    reg_pack->h.dh = row - 1;
    reg_pack->h.dl = col - 1;
    intr(int_video,reg_pack);
	
	free(reg_pack);
}
