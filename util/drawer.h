#ifndef UTIL_DRAWER_H
#define UTIL_DRAWER_H

#define DRAWER_INPUT_CHAR '_'
#define DRAWER_SEPARATOR_CHAR ' '
#define DRAWER_CHECKSUM_CHAR '_'
#define DRAWER_SPACER_CHAR ' '

char* drawer_input_page_draw_empty(unsigned short* line, unsigned short* column, unsigned short input_char_num, unsigned short separator_char_num, unsigned short checksum_char_num, unsigned short blocks_per_line, unsigned short total_block_num, unsigned short target_line_width);

char* drawer_input_page_draw_filled(unsigned short* line, unsigned short* column, char* input, unsigned short input_length, unsigned short input_char_num, unsigned short separator_char_num, unsigned short checksum_char_num, unsigned short blocks_per_line, unsigned short total_block_num, unsigned short target_line_width);

char* drawer_input_page_draw_input_at_index(unsigned short* line, unsigned short* column, char input, unsigned short input_index, unsigned short input_char_num, unsigned short separator_char_num, unsigned short checksum_char_num, unsigned short blocks_per_line, unsigned short total_block_num, unsigned short target_line_width);

char* drawer_input_page_continue_input_at_index(unsigned short* line, unsigned short* column, unsigned short input_index, unsigned short input_char_num, unsigned short separator_char_num, unsigned short checksum_char_num, unsigned short blocks_per_line, unsigned short total_block_num, unsigned short target_line_width);

char* drawer_input_page_cleanup(unsigned short line, unsigned short column, unsigned short blocks_per_line, unsigned short total_block_num);

#endif