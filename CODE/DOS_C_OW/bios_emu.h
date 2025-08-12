#ifndef __bios_emu__
#define __bios_emu__

#include <dos.h>

char far* bios_far_ptr(unsigned int my_segment, unsigned int my_offset){
    // char far* ptr = XXXXYYYY where XXXX is segment, YYYY is offset
    // bit shifts require integral which requires temporary casts to unsigned long
    return (char far*)\
    (
        (((unsigned long)my_segment) << 16) |\
        (((unsigned long)my_offset))
    );
}

unsigned int rol(unsigned int value, unsigned int shifts){
    while(shifts > 0){
        value = (value << 1) || ((value >>15) & 1);
        shifts--;
    }
    return value;
}

unsigned int ror(unsigned int value, unsigned int shifts){
    while (shifts > 0){
        value = ((value & 1) << 15)  | (value >> 1);
        shifts--;
    }
    return value;
}

unsigned char get_video_mode(){
	unsigned char mode;
	union REGPACK* reg_pack;
	reg_pack = malloc(sizeof(union REGPACK)); // Allocate memory
	
	reg_pack->h.ah = (unsigned char)int_video_get_mode;
	intr(int_video,reg_pack); // Get current video mode
	
	mode = reg_pack->h.al;
	
	free(reg_pack); // Free memory
	return mode;
}

void set_video_mode(unsigned char mode){
	union REGPACK* reg_pack;
	reg_pack = malloc(sizeof(union REGPACK)); // Allocate memory
	reg_pack->h.ah = (unsigned char)int_video_set_mode;
	reg_pack->h.al = mode;
	intr(int_video,reg_pack); // Get current video mode
	
	free(reg_pack); // Free memory
}


// PCjr Technical Reference - Test 4 - F000:0134 (A-9) and F000:FEEB (A-107)
unsigned int bios_ros_cksum(unsigned int my_segment, unsigned int my_offset, unsigned int my_chunk_size){
    unsigned int result;
    char sum;
    char far* my_far_ptr;

    my_far_ptr = bios_far_ptr(my_segment,my_offset);
    sum = 0;
    while(my_chunk_size > 0){
        sum+=(*my_far_ptr++);
        --my_chunk_size;
    }
    result = (unsigned int)sum;
    if(sum == 0){
        fprintf(stdout,"Checksum: PASS\r\n");
    }
    else {
        fprintf(stdout,"Checksum: FAIL\r\n");
    }
    return result;
}

// X^16 + X^12 + X^5 + 1
// DS = data segment of ROM space to be checked
// SI = index offset into ds pointing to 1st byte
// CX = length of space to be checked (including CRC bytres)
// On Exit:
// zero flag = set = CRC checked ok
// AH = 00
// AL = ??
// BX = 0000
// CL = 04
// DX = 0000 if crc checked ok, else, accumulated crc
// SI = (SI(ENTRY)+BX(ENTRY)
unsigned int bios_crc_check(unsigned int my_segment, unsigned int my_offset, unsigned int my_chunk_size){
    // PCjr Technical Reference - A-106 CRC_CHECK F000:FE71
    unsigned char tmp;
    unsigned int result;
    char far* far_ptr;
    union REGPACK* reg_pack;

    // Allocate the reg_pack memory
    reg_pack = malloc(sizeof(union REGPACK));

    // Setup inputs
    far_ptr = bios_far_ptr(my_segment,my_offset); // Pointer to memory
    reg_pack->w.ax = 0;
    reg_pack->w.bx = 0;
    reg_pack->w.cx = my_chunk_size;
    reg_pack->w.dx = 0;


    // CRC_CHECK
    reg_pack->w.bx = reg_pack->w.cx; // Save Count
    reg_pack->w.dx = 0xFFFF; // Init encode register
    // cld
    reg_pack->h.ah ^= reg_pack->h.ah; // Init work reg high
    reg_pack->h.cl = 4; // Get rotate count

    //CRC_1
    while(reg_pack->w.bx > 0){
        reg_pack->h.al = (*far_ptr++); // LODSB; Get byte
        reg_pack->h.dh ^= reg_pack->h.al; // XOR DH,AL; Form Aj + Cj + i
        reg_pack->h.al = reg_pack->h.dh; // MOV AL,DH
        reg_pack->w.ax = rol(reg_pack->w.ax,(unsigned int)(reg_pack->h.cl)); // ROL AX,CL; Shift work reg back 4

        reg_pack->w.dx ^= reg_pack->w.ax; // XOR DX,AX; add result into result reg
        reg_pack->w.ax = rol(reg_pack->w.ax,(unsigned int)1); // ROL AX,1 Shift work reg back 1

        // swap partial sum into result reg; XCHG DH, DL
        tmp = reg_pack->h.dh;
        reg_pack->h.dh = reg_pack->h.dl;
        reg_pack->h.dl = tmp;

        reg_pack->w.dx ^= reg_pack->w.ax; // XOR DX,AX; // AX Add work reg into results
        reg_pack->w.ax = ror(reg_pack->w.ax,(unsigned int)(reg_pack->h.cl)); // ROR AX,CL; Shift work reg over 4
        reg_pack->h.al &= 0xE0; // AND AL,0xE0; Clear off efgh
        reg_pack->w.dx ^= reg_pack->w.ax; // XOR DX,AX; add abcd into results
        reg_pack->w.ax = ror(reg_pack->w.ax,(unsigned int)1); // ROR AX,1; Shift work reg on over (AH = 0 for next pass)
        reg_pack->h.dh ^= reg_pack->h.al; // XOR SH,AL; add abcd back into results low
        reg_pack->w.bx -= 0x0001; // DEC BX; Decrement Count
    }
    reg_pack->w.dx |= reg_pack->w.dx; // Set zero flag accordingly, not used in the C code
    result = reg_pack->w.dx;

    free(reg_pack); // Free the regpack memory
    return result;
}

unsigned int bios_sum(unsigned int my_segment, unsigned int my_offset, unsigned int my_chunk_size){
    if(my_segment >= 0xD000 && my_segment < 0xF000){
        // TODO - use ROM checksum
        // A-21 + A-73
        return bios_crc_check(my_segment, my_offset, my_chunk_size);
    }
    else {
        // Uses ROS Checksum ( C000 < D000, > F000
        return bios_ros_cksum(my_segment, my_offset, my_chunk_size);
    }
}
#endif
