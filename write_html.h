#pragma once
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

enum ELEMENTS {
    TABLE_TITLE,
    TABLE_CELLS,
    TABLE_BODY_WITH_HEAD,
    TABLE_CELL_LEFT,
    TABLE_CELL_RIGHT_SPLIT,
    TABLE_CELL_RIGHT_SPAN,
    TABLE_ROW,
    TABLE_HEADING
};

int hprintf(int element, const char* format, ...);

void begin_table();
void end_table();
void begin_row();
void end_row();
void close_html_file();
FILE* create_html_file(const char* filename);
