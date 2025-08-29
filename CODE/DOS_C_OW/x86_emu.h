#ifndef __x86_emu__
#define __x86_emu__

// This module emulates some 8086/8088 instructions/helper functions that aren't natively implemented in C

unsigned int _x86_rol_16(unsigned int value, unsigned int shifts);
unsigned int _x86_ror_16(unsigned int value, unsigned int shifts);
char far* _x86_far_ptr(unsigned int segment, unsigned int offset);

#endif
