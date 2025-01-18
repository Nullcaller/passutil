#include<stdlib.h>
#include<string.h>

#include "transfer.h"

#include "util/drawer.h"

char* _transfer_receive_draw_comparison_table(char* format, char* key) {
	unsigned int comparisons_size = strlen(format);
	char** comparisons = malloc(sizeof(char*)*comparisons_size);

	unsigned int comparisons_format_length = strlen(TRANSFER_COMPARISONS_FORMAT);
	unsigned int comparison_length = comparisons_format_length + 2;
	for(unsigned int it = 0; it < comparisons_size; it++) {
		comparisons[it] = malloc(sizeof(char)*(comparison_length+1));
		comparisons[it][0] = key[it];
		memcpy(comparisons[it]+1, TRANSFER_COMPARISONS_FORMAT, comparisons_format_length);
		comparisons[it][comparisons_format_length+1] = format[it];
		comparisons[it][comparisons_format_length+2] = '\0';
	}

	char* comparisons_string = malloc(sizeof(char)*(comparison_length*comparisons_size+1));
	for(unsigned int it = 0; it < comparisons_size; it++)
		memcpy(comparisons_string+comparison_length*it, comparisons[it], comparison_length);
	comparisons_string[comparison_length*comparisons_size] = '\0';

	for(unsigned int it = 0; it < comparisons_size; it++)
		free(comparisons[it]);
	free(comparisons);

	unsigned short line, column;

	char* comparison_table = drawer_input_page_draw_filled(&line, &column, comparisons_string, strlen(comparisons_string), comparison_length, 0, 0, TRANSFER_COMPARISONS_PER_LINE, comparisons_size, TRANSFER_TARGET_LINE_WIDTH);
	unsigned int length = strlen(comparison_table);

	free(comparisons_string);

	comparison_table = realloc(comparison_table, length+2);
	comparison_table[length] = '\n';
	comparison_table[length+1] = '\0';

	return comparison_table;
}

char* _transfer_receive_undraw_comparison_table(char* format) {
	unsigned short total_block_num = strlen(format);
	unsigned short total_lines = total_block_num / TRANSFER_COMPARISONS_PER_LINE + (((total_block_num % TRANSFER_COMPARISONS_PER_LINE) > 0) ? 1 : 0);
	
	return drawer_input_page_cleanup(total_lines, 0, TRANSFER_COMPARISONS_PER_LINE, total_block_num);
}
