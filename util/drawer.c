#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "drawer.h"

#include "string.h"

void _drawer_fill_buf_with_char(char* buf, char character, unsigned short* start, unsigned short length) {
	for(unsigned short it = 0; it < length; it++)
		buf[*start+it] = character;
	*start += length;
}

unsigned short _drawer_input_page_calculate_block_length(unsigned short input_char_num, unsigned short separator_char_num, unsigned short checksum_char_num) {
	if(checksum_char_num <= 0)
		return input_char_num;
	else
		return input_char_num + separator_char_num + checksum_char_num;
}

void _drawer_input_page_draw_block(char* buf, unsigned short* start, unsigned short input_char_num, unsigned short separator_char_num, unsigned short checksum_char_num) {
	char* _buf = malloc(sizeof(char)*(_drawer_input_page_calculate_block_length(input_char_num, separator_char_num, checksum_char_num)));
	
	unsigned short len = 0;
	_drawer_fill_buf_with_char(_buf, DRAWER_INPUT_CHAR, &len, input_char_num);
	if(checksum_char_num > 0) {
		_drawer_fill_buf_with_char(_buf, DRAWER_SEPARATOR_CHAR, &len, separator_char_num);
		_drawer_fill_buf_with_char(_buf, DRAWER_CHECKSUM_CHAR, &len, checksum_char_num);
	}
	
	for(unsigned short it = 0; it < len; it++)
		buf[*start+it] = _buf[it];
	*start += len;

	free(_buf);
}

void _drawer_input_page_draw_block_line(char* buf, unsigned short* start, unsigned short input_char_num, unsigned short separator_char_num, unsigned short checksum_char_num, unsigned short blocks_per_line, unsigned short dud_block_num, unsigned short target_line_width) {
	unsigned short block_length = _drawer_input_page_calculate_block_length(input_char_num, separator_char_num, checksum_char_num);
	unsigned short spacing = target_line_width - block_length * blocks_per_line;
	unsigned short spacing_remainder = spacing % (blocks_per_line + 1);
	spacing /= blocks_per_line + 1;
	unsigned short len = 0;

	char* _buf = malloc(sizeof(char)*(target_line_width+1));

	_drawer_fill_buf_with_char(_buf, DRAWER_SPACER_CHAR, &len, spacing_remainder / 2);
	_drawer_fill_buf_with_char(_buf, DRAWER_SPACER_CHAR, &len, spacing);
	for(unsigned short it = 0; it < blocks_per_line; it++) {
		if(it < blocks_per_line-dud_block_num)
			_drawer_input_page_draw_block(_buf, &len, input_char_num, separator_char_num, checksum_char_num);
		else
			_drawer_fill_buf_with_char(_buf, DRAWER_SPACER_CHAR, &len, block_length);
		_drawer_fill_buf_with_char(_buf, DRAWER_SPACER_CHAR, &len, spacing);
	}
	_drawer_fill_buf_with_char(_buf, DRAWER_SPACER_CHAR, &len, spacing_remainder / 2 + spacing_remainder % 2);

	for(unsigned short it = 0; it < len; it++)
		buf[*start+it] = _buf[it];
	*start += len;

	free(_buf);
}

char* _drawer_input_page_move_cursor_from_pos_to_pos_vt100_esc(unsigned short line, unsigned short column, unsigned short target_line, unsigned short target_column) {
	char buf[256];
	unsigned short len = 0;

	if((target_line) > line)
		len += sprintf(buf+len, "\033[%dB", target_line-line);
	else if(target_line < line)
		len += sprintf(buf+len, "\033[%dA", line-target_line);

	if(target_column > column)
		len += sprintf(buf+len, "\033[%dC", target_column-column);
	else if(target_column < column)
		len += sprintf(buf+len, "\033[%dD", column-target_column);

	buf[len] = '\0';

	return strcpymalloc(buf);
}

int _drawer_input_page_calculate_input_pos(unsigned short* res_line, unsigned short* res_column, unsigned short input_index, unsigned short input_char_num, unsigned short separator_char_num, unsigned short checksum_char_num, unsigned short blocks_per_line, unsigned short total_block_num, unsigned short target_line_width) {
	unsigned short block_length = _drawer_input_page_calculate_block_length(input_char_num, separator_char_num, checksum_char_num);
	unsigned short input_chars_per_block = input_char_num + checksum_char_num;

	unsigned short input_block_index = input_index / input_chars_per_block;

	if(input_block_index >= total_block_num)
		return -1;

	unsigned short input_index_within_block = input_index % input_chars_per_block;

	unsigned short input_line_index = input_block_index / blocks_per_line;
	unsigned short input_block_index_within_line = input_block_index % blocks_per_line;

	unsigned short spacing = target_line_width - block_length * blocks_per_line;
	unsigned short spacing_remainder = spacing % (blocks_per_line + 1);
	spacing /= blocks_per_line + 1;

	unsigned short column = 0;
	column += spacing_remainder / 2;
	column += spacing;
	column += input_block_index_within_line*(block_length+spacing);
	column += (input_index_within_block >= input_char_num) ? (input_index_within_block + separator_char_num) : input_index_within_block;

	*res_line = input_line_index;
	*res_column = column;

	return 0;
}

char* drawer_input_page_draw_empty(unsigned short* line, unsigned short* column, unsigned short input_char_num, unsigned short separator_char_num, unsigned short checksum_char_num, unsigned short blocks_per_line, unsigned short total_block_num, unsigned short target_line_width) {
	unsigned short last_line_duds = blocks_per_line - total_block_num % blocks_per_line;
	unsigned short non_dud_lines = total_block_num / blocks_per_line;
	
	char* buf = malloc(sizeof(char)*(target_line_width+1)*(non_dud_lines+((last_line_duds > 0) ? 1 : 0)));
	unsigned short length = 0;

	for(unsigned short it = 0; it < non_dud_lines; it++) {
		_drawer_input_page_draw_block_line(buf, &length, input_char_num, separator_char_num, checksum_char_num, blocks_per_line, 0, target_line_width);
		buf[length] = '\n';
		length++;
	}

	if(last_line_duds > 0) {
		_drawer_input_page_draw_block_line(buf, &length, input_char_num, separator_char_num, checksum_char_num, blocks_per_line, last_line_duds, target_line_width);
		buf[length] = '\0';
		length++;
	} else
		buf[length-1] = '\0';

	*line += (non_dud_lines + ((last_line_duds > 0) ? 1 : 0) - 1);
	*column = target_line_width;

	return buf;
}

char* drawer_input_page_draw_filled(unsigned short* line, unsigned short* column, char* input, unsigned short input_length, unsigned short input_char_num, unsigned short separator_char_num, unsigned short checksum_char_num, unsigned short blocks_per_line, unsigned short total_block_num, unsigned short target_line_width) {
	unsigned short _line = 0, _column = 0;

	char* draw = drawer_input_page_draw_empty(&_line, &_column, input_char_num, separator_char_num, checksum_char_num, blocks_per_line, total_block_num, target_line_width);
	unsigned int draw_length = strlen(draw);
	
	char** char_draws = malloc(sizeof(char*)*input_length);
	unsigned int* char_draw_lengths = malloc(sizeof(unsigned int)*input_length);
	unsigned int len = draw_length;
	for(unsigned short it = 0; it < input_length; it++) {
		char_draws[it] = drawer_input_page_draw_input_at_index(&_line, &_column, input[it], it, input_char_num, separator_char_num, checksum_char_num, blocks_per_line, total_block_num, target_line_width);
		char_draw_lengths[it] = strlen(char_draws[it]);
		len += char_draw_lengths[it];
	}

	char* ret = malloc(sizeof(char)*(len+1));

	len = 0;
	memcpy(ret+len, draw, draw_length);
	free(draw);
	len += draw_length;
	for(unsigned short it = 0; it < input_length; it++) {
		memcpy(ret+len, char_draws[it], char_draw_lengths[it]);
		free(char_draws[it]);
		len += char_draw_lengths[it];
	}
	free(char_draws);
	free(char_draw_lengths);
	ret[len] = '\0';
	len += 1;

	*line = _line;
	*column = _column;

	return ret;
}

char* drawer_input_page_draw_input_at_index(unsigned short* line, unsigned short* column, char input, unsigned short input_index, unsigned short input_char_num, unsigned short separator_char_num, unsigned short checksum_char_num, unsigned short blocks_per_line, unsigned short total_block_num, unsigned short target_line_width) {
	unsigned short input_line = *line, input_column = *column;

	char* move = drawer_input_page_continue_input_at_index(&input_line, &input_column, input_index, input_char_num, separator_char_num, checksum_char_num, blocks_per_line, total_block_num, target_line_width);
	if(move == NULL)
		return NULL;

	unsigned int length = strlen(move);

	char* move_print = malloc(sizeof(char)*(length+2));
	memcpy(move_print, move, sizeof(char)*length);
	move_print[length] = input;
	move_print[length+1] = '\0';

	*line = input_line;
	*column = input_column+1;

	return move_print;
}

char* drawer_input_page_continue_input_at_index(unsigned short* line, unsigned short* column, unsigned short input_index, unsigned short input_char_num, unsigned short separator_char_num, unsigned short checksum_char_num, unsigned short blocks_per_line, unsigned short total_block_num, unsigned short target_line_width) {
	unsigned short input_line, input_column;

	if(_drawer_input_page_calculate_input_pos(&input_line, &input_column, input_index, input_char_num, separator_char_num, checksum_char_num, blocks_per_line, total_block_num, target_line_width) < 0)
		return NULL;

	char* move = _drawer_input_page_move_cursor_from_pos_to_pos_vt100_esc(*line, *column, input_line, input_column);

	*line = input_line;
	*column = input_column;

	return move;
}

char* drawer_input_page_cleanup(unsigned short line, unsigned short column, unsigned short input_char_num, unsigned short separator_char_num, unsigned short checksum_char_num, unsigned short blocks_per_line, unsigned short total_block_num, unsigned short target_line_width) {
	unsigned short line_count = total_block_num / blocks_per_line + (total_block_num % blocks_per_line > 0 ? 1 : 0);

	char* move = _drawer_input_page_move_cursor_from_pos_to_pos_vt100_esc(line, column, line_count-1, 0);
	unsigned short move_len = strlen(move);

	char* line_cleanup = "\033[2K";
	unsigned short line_cleanup_length = strlen(line_cleanup);
	char* line_up = "\033[A";
	unsigned short line_up_length = strlen(line_up);

	char* cleanup_sequence = malloc(sizeof(char)*(line_cleanup_length*line_count+line_up_length*(line_count-1)+1));
	unsigned int len = 0;

	for(unsigned short it = 1; it <= line_count; it++) {
		memcpy(cleanup_sequence+len, line_cleanup, line_cleanup_length);
		len += line_cleanup_length;
		if(it < line_count) {
			memcpy(cleanup_sequence+len, line_up, line_up_length);
			len += line_up_length;
		}
	}
	cleanup_sequence[len] = '\0';
	len += 1;

	char* move_cleanup_sequence = malloc(sizeof(char)*(move_len+len));
	memcpy(move_cleanup_sequence, move, move_len);
	memcpy(move_cleanup_sequence+move_len, cleanup_sequence, len);

	free(move);
	free(cleanup_sequence);

	return move_cleanup_sequence;
}
