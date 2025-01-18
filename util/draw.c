#include<stdio.h>
#include<stdlib.h>

#include "draw.h"

#include "util/string.h"

void _fill_buf_with_char(char* buf, char character, unsigned short* start, unsigned short length) {
	for(unsigned short it = 0; it < length; it++)
		buf[*start+it] = character;
	*start += length;
}

unsigned short _calculate_input_block_length(unsigned short input_char_num, unsigned short separator_char_num, unsigned short checksum_char_num) {
	if(checksum_char_num <= 0)
		return input_char_num;
	else
		return input_char_num + separator_char_num + checksum_char_num;
}

void _draw_input_block(char* buf, unsigned short* start, unsigned short input_char_num, unsigned short separator_char_num, unsigned short checksum_char_num) {
	char* _buf = malloc(sizeof(char)*(_calculate_input_block_length(input_char_num, separator_char_num, checksum_char_num)));
	
	unsigned short len = 0;
	_fill_buf_with_char(_buf, DRAW_INPUT_CHAR, &len, input_char_num);
	if(checksum_char_num > 0) {
		_fill_buf_with_char(_buf, DRAW_SEPARATOR_CHAR, &len, separator_char_num);
		_fill_buf_with_char(_buf, DRAW_CHECKSUM_CHAR, &len, checksum_char_num);
	}
	
	for(unsigned short it = 0; it < len; it++)
		buf[*start+it] = _buf[it];
	*start += len;

	free(_buf);
}

void _draw_input_block_line(char* buf, unsigned short* start, unsigned short input_char_num, unsigned short separator_char_num, unsigned short checksum_char_num, unsigned short blocks_per_line, unsigned short dud_block_num, unsigned short target_line_width) {
	unsigned short block_length = _calculate_input_block_length(input_char_num, separator_char_num, checksum_char_num);
	unsigned short spacing = target_line_width - block_length * blocks_per_line;
	unsigned short spacing_remainder = spacing % (blocks_per_line + 1);
	spacing /= blocks_per_line + 1;
	unsigned short len = 0;

	char* _buf = malloc(sizeof(char)*(target_line_width+1));

	_fill_buf_with_char(_buf, DRAW_SPACER_CHAR, &len, spacing_remainder / 2);
	_fill_buf_with_char(_buf, DRAW_SPACER_CHAR, &len, spacing);
	for(unsigned short it = 0; it < blocks_per_line; it++) {
		if(it < blocks_per_line-dud_block_num)
			_draw_input_block(_buf, &len, input_char_num, separator_char_num, checksum_char_num);
		else
			_fill_buf_with_char(_buf, DRAW_SPACER_CHAR, &len, block_length);
		_fill_buf_with_char(_buf, DRAW_SPACER_CHAR, &len, spacing);
	}
	_fill_buf_with_char(_buf, DRAW_SPACER_CHAR, &len, spacing_remainder / 2 + spacing_remainder % 2);

	for(unsigned short it = 0; it < len; it++)
		buf[*start+it] = _buf[it];
	*start += len;

	free(_buf);
}

char* _draw_input_page(unsigned short input_char_num, unsigned short separator_char_num, unsigned short checksum_char_num, unsigned short blocks_per_line, unsigned short total_block_num, unsigned short target_line_width) {
	unsigned short last_line_duds = total_block_num % blocks_per_line;
	unsigned short non_dud_lines = total_block_num / blocks_per_line;
	
	char* buf = malloc(sizeof(char)*(target_line_width+1)*(non_dud_lines+((last_line_duds > 0) ? 1 : 0)));
	unsigned short length = 0;

	for(unsigned short it = 0; it < non_dud_lines; it++) {
		_draw_input_block_line(buf, &length, input_char_num, separator_char_num, checksum_char_num, blocks_per_line, 0, target_line_width);
		buf[length] = '\n';
		length++;
	}

	if(last_line_duds > 0) {
		_draw_input_block_line(buf, &length, input_char_num, separator_char_num, checksum_char_num, blocks_per_line, last_line_duds, target_line_width);
		buf[length] = '\0';
		length++;
	} else
		buf[length-1] = '\0';

	return buf;
}

char* _input_page_move_cursor_from_pos_to_pos(unsigned short line, unsigned short column, unsigned short target_line, unsigned short target_column) {
	char buf[256];
	unsigned short len = 0;

	if(target_line > line)
		len += sprintf(buf+len, "\033[%dB", target_line-line);
	else if(target_line < line)
		len += sprintf(buf+len, "\033[%dA", line-target_line);
	
	if(target_column > column)
		len += sprintf(buf+len, "\033[%dC", target_column-column);
	else if(column < target_column)
		len += sprintf(buf+len, "\033[%dD", column-target_column);

	return strcpymalloc(buf);
}
