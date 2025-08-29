// Standard C includes
#include <stdlib.h> // For malloc

// Platform specific includes
#include <dos.h>

// Project specific includes
#include "x86_emu.h"

// Module specific includes
#include "bios_emu.h"

unsigned char _bios_get_video_mode(){
	unsigned char mode;
	union REGPACK* reg_pack;
	reg_pack = (union REGPACK*)malloc(sizeof(union REGPACK)); // Allocate memory
	
	reg_pack->h.ah = (unsigned char)int_video_get_mode;
	intr(int_video,reg_pack); // Get current video mode
	
	mode = reg_pack->h.al; // mode to return
	
	free(reg_pack); // Free memory
	return mode;
}

void _bios_set_video_mode(unsigned char mode){
	union REGPACK* reg_pack;
	reg_pack = malloc(sizeof(union REGPACK)); // Allocate memory
	
	reg_pack->h.ah = (unsigned char)int_video_get_mode;
	intr(int_video,reg_pack); // Get current video mode to setup other regs
	
	reg_pack->h.ah = (unsigned char)int_video_set_mode;
	reg_pack->h.al = mode;
	intr(int_video,reg_pack); // Set current video mode
	
	free(reg_pack); // Free memory
}

// PCjr Technical Reference - Test 4 - F000:0134 (A-9) and F000:FEEB (A-107)
unsigned int _bios_ros_cksum(unsigned int segment, unsigned int offset, unsigned int chunk_size){
    unsigned int result;
    char sum;
    char far* far_ptr;

    far_ptr = _x86_far_ptr(segment,offset);
    sum = 0;
    while(chunk_size > 0){
        sum+=(*far_ptr++);
        --chunk_size;
    }
    result = (unsigned int)sum;
	/*
    if(sum == 0){
        fprintf(stdout,"Checksum: PASS\r\n");
    }
    else {
        fprintf(stdout,"Checksum: FAIL\r\n");
    }
	*/
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
unsigned int _bios_crc_check(unsigned int segment, unsigned int offset, unsigned int chunk_size){
    // PCjr Technical Reference - A-106 CRC_CHECK F000:FE71
    unsigned char tmp;
    unsigned int result;
    char far* far_ptr;
    union REGPACK* reg_pack;

    // Allocate the reg_pack memory
    reg_pack = (union REGPACK*)malloc(sizeof(union REGPACK));

    // Setup inputs
    far_ptr = _x86_far_ptr(segment,offset); // Pointer to memory
    reg_pack->w.ax = 0;
    reg_pack->w.bx = 0;
    reg_pack->w.cx = chunk_size;
    reg_pack->w.dx = 0;


    // CRC_CHECK
    reg_pack->w.bx = reg_pack->w.cx; // Save Count
    reg_pack->w.dx = 0xFFFF; // Initialize encode register
    // cld
    reg_pack->h.ah ^= reg_pack->h.ah; // Init work reg high
    reg_pack->h.cl = 4; // Get rotate count

    //CRC_1
    while(reg_pack->w.bx > 0){
        reg_pack->h.al = (*far_ptr++); // LODSB; Get byte
        reg_pack->h.dh ^= reg_pack->h.al; // XOR DH,AL; Form Aj + Cj + i
        reg_pack->h.al = reg_pack->h.dh; // MOV AL,DH
        reg_pack->w.ax = _x86_rol_16(reg_pack->w.ax,(unsigned int)(reg_pack->h.cl)); // ROL AX,CL; Shift work reg back 4

        reg_pack->w.dx ^= reg_pack->w.ax; // XOR DX,AX; add result into result reg
        reg_pack->w.ax = _x86_rol_16(reg_pack->w.ax,(unsigned int)1); // ROL AX,1 Shift work reg back 1

        // swap partial sum into result reg; XCHG DH, DL
        tmp = reg_pack->h.dh;
        reg_pack->h.dh = reg_pack->h.dl;
        reg_pack->h.dl = tmp;

        reg_pack->w.dx ^= reg_pack->w.ax; // XOR DX,AX; // AX Add work reg into results
        reg_pack->w.ax = _x86_ror_16(reg_pack->w.ax,(unsigned int)(reg_pack->h.cl)); // ROR AX,CL; Shift work reg over 4
        reg_pack->h.al &= 0xE0; // AND AL,0xE0; Clear off efgh
        reg_pack->w.dx ^= reg_pack->w.ax; // XOR DX,AX; add abcd into results
        reg_pack->w.ax = _x86_ror_16(reg_pack->w.ax,(unsigned int)1); // ROR AX,1; Shift work reg on over (AH = 0 for next pass)
        reg_pack->h.dh ^= reg_pack->h.al; // XOR SH,AL; add abcd back into results low
        reg_pack->w.bx -= 0x0001; // DEC BX; Decrement Count
    }
    reg_pack->w.dx |= reg_pack->w.dx; // Set zero flag accordingly, not used in the C code
    result = reg_pack->w.dx;

    free(reg_pack); // Free the regpack memory
    return result;
}

unsigned int _bios_sum(unsigned int segment, unsigned int offset, unsigned int chunk_size){
    if(segment >= 0xD000 && segment < 0xF000){
        // TODO - use ROM checksum
        // A-21 + A-73
        return _bios_crc_check(segment, offset, chunk_size);
    }
    else {
        // Uses ROS Checksum ( C000 < D000, > F000
        return _bios_ros_cksum(segment, offset, chunk_size);
    }
}
