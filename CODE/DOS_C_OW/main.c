/* Standard C Libraries */
#include <stdio.h> /* For printing */
#include <stdlib.h> /* For malloc */

/* DOS/BIOS specific includes */
#include <conio.h> /* For reading the keyboard directly */
#include <dos.h> /* NOTE: look in i86.h [included when dos.h is included] for register structures and interrupt call definitions */

/* Project includes */
#include "bios.h"

/* Definitions */
#define int_keyboard 0x16
#define     int_keyboard_read_key 0x00
#define     int_keyboard_get_state 0x01

// V1 - just unconditionally dump cartridge spaces in 32K chunks
// #V2 - before dumping, check to see any signature/contents and ask


unsigned int segment;
unsigned int offset;
unsigned long chunk_size;
char* working_file_name;
char far* far_ptr;

void dump_chunk(unsigned int my_segment, unsigned int my_offset, unsigned long my_chunk_size, char* my_file_name){
    FILE* my_working_file;
    far_ptr = bios_far_ptr(my_segment,my_offset);
    my_working_file = fopen(my_file_name,"wb");
    while(my_chunk_size > 0){
        fprintf(my_working_file,"%c",(*far_ptr++));
        --my_chunk_size;
    }
    fclose(my_working_file);
}

int main(int argc, char* argv[]){
    fprintf(stdout,"PCjrDUMP\r\n");
    fprintf(stdout,"Intended to dump the possible cartridge spaces in 32K chunks\r\n");
    fprintf(stdout,"Note: this does not currently attempt to determine if the cartridge is 32K or\r\n");
    fprintf(stdout,"64K so you may have to combine two files together to get a full image...\r\n");

    // Set initial segment/offset
    segment = 0xD000;
    offset = 0x0000;
    chunk_size = 0x8000; // 32K

    // Allocate filename space
    working_file_name = (char*)malloc( (8+1+3+1) * sizeof(char) ); // filename.bin\0 = 8 + 1 + 3 + 1

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
    return 0;
}
