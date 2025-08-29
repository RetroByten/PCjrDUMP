#ifndef __bios_defines__
#define __bios_defines__

#define int_keyboard 0x16
#define     int_keyboard_read_key 0x00
#define     int_keyboard_get_state 0x01

#define int_video 0x10
#define 	int_video_set_mode 0x00
#define 		video_mode_4025_greyscale 0x00
#define 		video_mode_4025_color 0x01
#define 		video_mode_8025_greyscale 0x02
#define int_video_set_cursor_pos 0x02
#define int_video_get_cursor_pos 0x03
#define 	int_video_print_tty 0x0E
#define 	int_video_get_mode 0x0F

#define			video_color_white 0x0F

#endif
