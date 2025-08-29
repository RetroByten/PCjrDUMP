#ifndef __basic_emu__
#define __basic_emu__

void _basic_cls();
void _basic_print_newline();
void _basic_print_char(char c);
void _basic_print(char* s);
void _basic_println(char* s);
void _basic_print_hex_8(unsigned char hex_byte);
void _basic_print_hex_16(unsigned int hex_word);
void _basic_locate(char row, char col);

#endif




