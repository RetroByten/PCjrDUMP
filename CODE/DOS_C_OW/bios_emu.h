#ifndef __bios_emu__
#define __bios_emu__

// Module-specific includes
#include "bios_defines.h" // Needed for modules using the BIOS

// Provides an interface to portions of the BIOS

unsigned char _bios_get_video_mode();
void _bios_set_video_mode(unsigned char);
unsigned int _bios_ros_cksum(unsigned int segment, unsigned int offset, unsigned int chunk_size);
unsigned int _bios_crc_check(unsigned int segment, unsigned int offset, unsigned int chunk_size);
unsigned int _bios_sum(unsigned int segment, unsigned int offset, unsigned int chunk_size);

#endif
