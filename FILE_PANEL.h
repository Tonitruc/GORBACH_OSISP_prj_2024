#pragma once

#include <stdbool.h>
#include <ncurses.h>
#include <menu.h>

#define START_DIR "/home/tonitrus/Projects/C"

#define COLS_PERCENT 0.50
#define ROWS_PERCENT 0.20

#define FILE_PANEL_SIZE(height, width) height = (LINES - ROWS_PERCENT * LINES); width = (COLS - COLS_PERCENT * COLS);   
#define PANEL_START_POS(starty, startx, num) starty = ROWS_PERCENT * LINES; startx = 0 + num * COLS * COLS_PERCENT;

typedef struct _FILE_PANEL {
    WINDOW* panel;
    char* current_directory;
    MENU *file_menu;
} FILE_PANEL;

bool init_file_panel(FILE_PANEL** file_panel, int num);
bool resize_file_panel(FILE_PANEL* file_panel, int num);
void show_file_panel(FILE_PANEL *file_panel);
void load_start_dir(FILE_PANEL* file_panel);

