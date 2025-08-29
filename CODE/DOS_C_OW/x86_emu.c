// Module specific includes
#include "x86_emu.h"

unsigned int _x86_rol_16(unsigned int value, unsigned int shifts){
    while(shifts > 0){
        value = (value << 1) | ((value >>15) & 1);
        shifts--;
    }
    return value;
}

unsigned int _x86_ror_16(unsigned int value, unsigned int shifts){
    while (shifts > 0){
        value = ((value & 1) << 15) | (value >> 1);
        shifts--;
    }
    return value;
}

char far* _x86_far_ptr(unsigned int segment, unsigned int offset){
    // char far* ptr = XXXXYYYY where XXXX is segment, YYYY is offset
    // bit shifts require integral which requires temporary casts to unsigned long
    return (char far*)\
    (
        (((unsigned long)segment) << 16) |\
        (((unsigned long)offset))
    );
}
