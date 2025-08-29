/* Constant Definitions */
#define int_keyboard 0x16
#define     int_keyboard_read_key 0x00
#define     int_keyboard_get_state 0x01

#define int_video 0x10
#define 	int_video_set_mode 0x00
//#define 		video_mode_4025_color 0x01
#define 		video_mode_4025_greyscale 0x00
//#define mode_8025_greyscale 0x02
#define int_video_set_cursor_pos 0x02
#define int_video_get_cursor_pos 0x03
#define 	int_video_get_mode 0x0F

/* Program behavior definitions */
#define b_video_mode video_mode_4025_greyscale
#define b_memory_bios_segment 0xF000
#define b_memory_bios_machine_id 0xFFFE

/* Standard C Libraries */
#include <stdio.h> /* For printing */
#include <stdlib.h> /* For malloc */

/* DOS/BIOS specific includes */
#include <conio.h> /* For reading the keyboard directly */
#include <dos.h> /* NOTE: look in i86.h [included when dos.h is included] for register structures and interrupt call definitions */

/* Project includes */
#include "bios_emu.h"



enum {
        TITLE_MODE = 0,
        MENU_MODE,
        CREDITS_MODE,
        EXIT_MODE
};


// V1 - just unconditionally dump cartridge spaces in 32K chunks
// V2 - Structure:
//      - 1.) Title screen mode
//  - 2.) Menu screen mode
//  - 3x.) Option Sub-screen mode(s)
//  - 4.) Credits screen mode
//  - 5.) Exit mode

// #VN - before dumping, check to see any signature/contents and ask



char get_key_press(){
        return getch(); // conio.h
}

void wait_for_any_key_press(){
        get_key_press(); // toss out result because we don't care which key
}



void dump_chunk(unsigned int my_segment, unsigned int my_offset, unsigned int my_chunk_size, char* my_file_name){
    FILE* my_working_file;
	char far* far_ptr;
    far_ptr = bios_far_ptr(my_segment,my_offset);
    my_working_file = fopen(my_file_name,"wb");
    while(my_chunk_size > 0){
        fprintf(my_working_file,"%c",(*far_ptr++));
        --my_chunk_size;
    }
    fclose(my_working_file);
}

unsigned int title(){
	unsigned int local_mode;
	unsigned char machine_id;
	char machine_char;
	char far* far_ptr;

	// Get current machine id
	far_ptr = bios_far_ptr(b_memory_bios_segment,b_memory_bios_machine_id);
	machine_id = *far_ptr;


	// Screen
	basic_cls();
	basic_locate(10,1);
	basic_print("main title");
	
	basic_locate(11,5);
	basic_print("By: RetroByten");
	
	wait_for_any_key_press();

	local_mode = MENU_MODE;
	return local_mode;
	//
	
	// Screen prints
	//bios_video_set_xy((unsigned char)0,(unsigned char)0);
	printf("main::title()");
	
	//bios_video_set_xy(0,1);
	printf("PCjrDUMP");
	
	//bios_video_set_xy(2,2);
	fprintf(stdout," Examine/Dump ROM memory");
	//bios_video_set_xy(2,3);
	fprintf(stdout," Segments: C000 - FFFF");
	
	//bios_video_set_xy(0,5);
	fprintf(stdout,"Current machine_id: %02X",machine_id);
	//bios_video_set_xy(0,6);
	fprintf(stdout,"Machine Lookup: ");
	switch(machine_id){
		case 0xFF:
			machine_char = 0xE0;
			fprintf(stdout,"IBM PC (5150)");
			break;
		case 0xFE:
			machine_char = 0xE1;
			fprintf(stdout,"IBM PC XT (5160)|PC Portable (5155)");
			break;
		case 0xFD:
			machine_char = 0x01;
			fprintf(stdout,"IBM PCjr (4860)");
			break;
		case 0xFC:
			machine_char = 0xE3;
			fprintf(stdout,"IBM PC AT (5170)");
			break;
		default:
			machine_char = 0x3F;
			fprintf(stdout,"UNK (####)");
			break;
	}
	
	fprintf(stdout," - %c",machine_char);
	
	//bios_video_set_xy(0,9);
	fprintf(stdout,"Press any key to continue...");

	wait_for_any_key_press();

	local_mode = MENU_MODE;
	return local_mode;
}

unsigned int menu(){
        unsigned int local_mode;
        fprintf(stdout,"main::menu()\r\n");


        local_mode = EXIT_MODE;
        return local_mode;
}




//int main(int argc, char* argv[]){
unsigned int program_mode;
unsigned char previous_video_mode;
unsigned int segment;
unsigned int offset;
unsigned int chunk_size;
char* working_file_name;

int main(){
		previous_video_mode = get_video_mode(); // Save off video mode

        program_mode = TITLE_MODE; // Set Initial Mode
        do { // Main Program Loop
                switch(program_mode){
                        case TITLE_MODE:
                                program_mode = title();
                                break;
                        case MENU_MODE:
                                program_mode = menu();
                                break;
                        default:
                                program_mode = EXIT_MODE;
                                break;
                }
        } while (program_mode != EXIT_MODE);
		
		set_video_mode(previous_video_mode); // Restore video
		
        return 0;
}
/*
    fprintf(stdout,"PCjrDUMP\r\n");
    fprintf(stdout,"Intended to dump the possible cartridge spaces in 32K chunks\r\n");
    fprintf(stdout,"Note: this does not currently attempt to determine if the cartridge is 32K or\r\n");
    fprintf(stdout,"64K so you may have to combine two files together to get a full image...\r\n");

    // Set initial segment/offset
    segment = 0xD000;
    offset = 0x0000;
    chunk_size = 0x8000; // 32K

    // Allocate filename space
    working_file_name = (char*)(malloc((8+1+3+1) * sizeof(char))); // filename.bin\0 = 8 + 1 + 3 + 1

    // Process the segments
    while (segment >= 0xD000){
        sprintf(working_file_name,"%04X%04X.bin",segment,offset,chunk_size);
        fprintf(stdout,"Processing %s\r\n",working_file_name);
        bios_sum(segment,offset,chunk_size);
        dump_chunk(segment,offset,chunk_size,working_file_name);
        segment += 0x800;
        offset = 0x0000;
        chunk_size = 0x8000;
    }

    free(working_file_name);
*/

/*
Notes - 40 column

---
- PCjrDUMP\tv0.1\tRetroByten
---

[LEFT]
---
- D000:0000-7FFF
---
- xx xx xx xx xx xx xx xx 
- xx xx xx xx xx xx xx xx
- xx xx xx xx xx xx xx xx
- xx xx xx xx xx xx xx xx
---

---


*/
