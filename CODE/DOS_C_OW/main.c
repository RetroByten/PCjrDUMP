
// Standard C includes
#include <stdio.h> // For printing
//#include <stdlib.h> // For malloc

// DOS / BIOS specific includes
#include <conio.h> // For reading the keyboard directly // TODO - move this to BIOS
//#include <dos.h>

// Project specific includes
#include "x86_emu.h"
#include "bios_emu.h" // Emulates some BIOS functions
#include "basic_emu.h" // Emulates some BASIC functions

// Enums
enum {
        TITLE_MODE = 0,
        MENU_MODE,
        CREDITS_MODE,
        EXIT_MODE
};

// Globals
unsigned char original_video_mode;
unsigned char main_program_mode;

// Utility Functions // TODO - switch these for bios_emu
char get_key_press(){
	return getch(); // conio.h
}

void wait_for_any_key_press(){
	get_key_press(); // toss out result because we don't care which key
}


// Core Functions
unsigned char title(){
	_basic_cls();
	_basic_locate(1,1);
	_basic_println("PCjrDUMP - RetroByten");
	_basic_locate(2,2);
	_basic_print("Examine/Dump ROM areas");	
	_basic_locate(4,1);
	_basic_print("Press enter key to continue...");

	wait_for_any_key_press();
	return MENU_MODE;
}

void print_memory_block(unsigned int segment, unsigned int offset, unsigned int count){
	char far* far_ptr;
	
	far_ptr = _x86_far_ptr(segment,offset);
	while( count > 0 ){
		_basic_print_hex_8(*far_ptr++);
		count--;
	}
}

void print_header(unsigned int segment, unsigned int offset, unsigned int count){
	_basic_locate(1,1);
	_basic_print("MEMORY - ");
	_basic_print_hex_16(segment);
	_basic_print_char(':');
	_basic_print_hex_16(offset);
	_basic_print_char(',');
	_basic_print_hex_16(count);
}

void dump_memory(unsigned int segment, unsigned int offset, unsigned int count){
	char far* far_ptr;
	char* file_name;
	FILE* working_file;
	
	file_name = "00000000.xxx";
	sprintf(file_name,"%04X",segment);
	sprintf(file_name+4,"%04X",offset);
	sprintf(file_name+8,".bin");
	
	far_ptr = _x86_far_ptr(segment,offset);
	working_file = fopen(file_name,"wb");
	while (count > 0){
		fprintf(working_file,"%c",*far_ptr++);
	}
	
	fclose(working_file);
}

void memory_keypress(unsigned int segment, unsigned int offset, unsigned int count){
	char input;

	_basic_locate(10,1);
	_basic_print("Press 'D' to dump to ");
	_basic_print_hex_16(segment);
	_basic_print_hex_16(offset);
	_basic_println(".bin");
	_basic_println("Press 'N' to move to next block...");
	
	input = 'a';
	do {
		input = get_key_press();
		if ( input == 'd' ){
			_basic_print("Dumping...");
			dump_memory(segment,offset,count);
			_basic_print("Done! Moving on...");
			input = 'n';
		}
	} while(input != 'n' && input != 'N');
}

void test_4_planar_board_ros_cksum(){ // PCjr Technical Reference (A-9)
	unsigned int segment; // DS
	unsigned int offset; // SI
	unsigned int count; // CX
	
	unsigned int working_segment;
	unsigned int working_offset;
	unsigned int working_count;
	
	unsigned char result;
	
	// LOW
	segment = 0xF000;
	working_segment = segment;
	offset = 0x0000;
	working_offset = offset;
	count = 0x8000;
	working_count = count;

	// Reset screen
	_basic_cls();
	
	// Print header
	print_header(segment,offset,count);
	_basic_print_newline();
	
	// Print 16 bytes
	print_memory_block(segment,offset,16);
	_basic_print_newline();
	
	// Check Low, then High
	result = _bios_ros_cksum(&working_segment,&working_offset,&working_count);
	_basic_print("Checksum - ");
	if(result == 0){
		_basic_print("Pass");
	}
	else {
		_basic_print("Fail");
	}
	_basic_print_newline();
	
	// TODO - Add CRC
	_basic_print("CRC - ");
	_basic_print_newline();
	
	// Menu options
	memory_keypress(segment,offset,count);
	
	
	// HIGH
	// segment/working_segment hasn't changed
	offset = working_offset;
	count = 0x8000;
	working_count = count;
	
	// Reset screen
	_basic_cls();
	
	// Print header
	print_header(segment,offset,count);
	_basic_print_newline();
	
	// Print 16 bytes
	print_memory_block(segment,offset,16);
	_basic_print_newline();
	
	// Check Low, then High
	result = _bios_ros_cksum(&working_segment,&working_offset,&working_count);
	_basic_print("Checksum - ");
	if(result == 0){
		_basic_print("Pass");
	}
	else {
		_basic_print("Fail");
	}
	memory_keypress(segment,offset,count);
}


unsigned char menu(){
	test_4_planar_board_ros_cksum(); // PCjr Technical Reference (A-9)
		
	// C000
	
	// D000-E800


	
	return CREDITS_MODE;
	/*
	char far* far_ptr;
	unsigned int segment;
	unsigned int offset;
	unsigned char count;

	unsigned int signature;
	unsigned char length;
	unsigned char instruction;
	
	// Relies on wrap-around for 0xD000 - 0xF800
	for ( segment = 0xC000; segment >= 0xC000; segment += 0x800 ){ 
		offset = 0;
		
		_basic_cls();
		
		// Print header
		_basic_locate(1,1);
		_basic_print("MEMORY - ");
		_basic_print_hex_16(segment);
		_basic_print_newline();
		
		if (segment >= 0xD000 && segment < 0xF000){ // Check cartridge for header
			offset = 0x0000;
			_basic_print("OFFSET - ");
			_basic_print_hex_16(offset);
			_basic_print_newline();
		
			far_ptr = _x86_far_ptr(segment,offset);
			// Print Raw header area
			for (count = 0; count < 16; count++){
				_basic_print_hex_8(*(far_ptr + (unsigned long)count));
			}
			_basic_print_newline();
		
			_basic_print("Signature - ");
			signature = (*(far_ptr + (unsigned long)1)) << 8 |\
						(*(far_ptr + (unsigned long)0));
						
			if(signature == 0xAA55){
				_basic_println("PRESENT");
				length = *(far_ptr + 2);
				instruction = *(far_ptr + (unsigned long)3);
				
				_basic_print("Length - ");
				_basic_print_hex_8(length);
				_basic_print(" - Entry - ");
				switch(instruction){
					case 0xE9:
						_basic_print("JMP near ");
						_basic_print_hex_16((*(far_ptr + (unsigned long)5)) << 8 |\
											(*(far_ptr + (unsigned long)4))
						);
						break;
					case 0xEB:
						_basic_print("JMP short ");
						_basic_print_hex_8(*(far_ptr + (unsigned long)4));
						_basic_print(" , UNK - ");
						_basic_print_hex_8(*(far_ptr + (unsigned long)5));
						break;
					case 0xCB:
						_basic_print("RETF - ");
						break;
					default:
						_basic_print("Unknown - ");
						_basic_print_hex_8(instruction);
						break;
				}				
			}
			else {
				_basic_println("ABSENT");
			}
			
		}
		*/
		/*
		// Print first 16 bytes
		far_ptr = _x86_far_ptr(segment,offset);
		_basic_print("Raw: ");
		for ( count = 0; count < 16; count++ ){
			_basic_print_hex_8(*far_ptr++);
		}
		_basic_print_newline();
		
		

		
		//wait_for_any_key_press();
	}



	
	
	_basic_print("Press any key to begin exit...");
	wait_for_any_key_press();
	return CREDITS_MODE;
	*/
}

unsigned char credits(){
	_basic_cls();
	_basic_locate(1,1);
	_basic_print("CREDITS");
	_basic_locate(4,1);
	_basic_print("Programming - Ryan Paterson");
	_basic_locate(5,1);
	_basic_print("Press any key to finish exit...");
	wait_for_any_key_press();
	return EXIT_MODE;
}

int main(){
	
	// Set stdout to unbuffered
	setbuf(stdout, NULL);
	
	original_video_mode = _bios_get_video_mode(); // Save user's original video mode
	_bios_set_video_mode(video_mode_4025_color); // Set video mode to lowest common color denominator
	
	main_program_mode = TITLE_MODE; // Set initial title mode
	do { // Main Program Loop
		switch(main_program_mode){
			case TITLE_MODE:
				main_program_mode = title();
				break;
			case MENU_MODE:
				main_program_mode = menu();
				break;
			case CREDITS_MODE:
				main_program_mode = credits();
				break;
			default:
				main_program_mode = EXIT_MODE;
				break;
			}
	} while (main_program_mode != EXIT_MODE);
	
	_bios_set_video_mode(original_video_mode); // Restore original video
	return 0;
}



