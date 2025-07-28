#ifndef __bios__
#define __bios__

char far* bios_far_ptr(unsigned int my_segment, unsigned int my_offset){
    // char far* ptr = XXXXYYYY where XXXX is segment, YYYY is offset
    // bit shifts require integral which requires temporary casts to unsigned long
    return (char far*)\
    (
        (((unsigned long)my_segment) << 16) |\
        (((unsigned long)my_offset))
    );
}

void bios_ros_cksum(unsigned int my_segment, unsigned int my_offset, unsigned long my_chunk_size){
    // PCjr Technical Reference - Test 4 - F000:0134 (A-9) and F000:FEEB (A-107)
    char sum;
    char far* my_far_ptr;


    my_far_ptr = bios_far_ptr(my_segment,my_offset);
    sum = 0;
    while(my_chunk_size > 0){
        sum+=(*my_far_ptr++);
        --my_chunk_size;
    }

    if(sum == 0){
        fprintf(stdout,"Checksum: PASS\r\n");
    }
    else {
        fprintf(stdout,"Checksum: FAIL\r\n");
    }
}

void bios_crc(unsigned int my_segment, unsigned int my_offset, unsigned long my_chunk_size){
    fprintf(stdout,"Skipping CRC\r\n");
}

void bios_sum(unsigned int my_segment, unsigned int my_offset, unsigned long my_chunk_size){
    if(my_segment >= 0xD000 && my_segment < 0xF000){
        // TODO - use ROM checksum
        // A-21 + A-73
        bios_crc(my_segment, my_offset, my_chunk_size);
    }
    else {
        // Uses ROS Checksum ( C000 < D000, > F000
        bios_ros_cksum(my_segment, my_offset, my_chunk_size);
    }
}
#endif
