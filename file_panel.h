#pragma once

#include <stdbool.h>
#include <ncurses.h>
#include <menu.h>

#define START_DIR "/home/tonitrus/Projects/C"

#define COLS_PERCENT 0.50
#define ROWS_PERCENT 0.20

#define FILE_PANEL_SIZE(height, width) height = (LINES - ROWS_PERCENT * LINES); width = (COLS - COLS_PERCENT * COLS);   
#define PANEL_START_POS(starty, startx, num) starty = ROWS_PERCENT * LINES; startx = 0 + num * COLS * COLS_PERCENT;

#define FILE_MENU_START_ROW 3
#define FILE_MENU_START_COL 1
#define FILE_MENU_FORMAT_ROW 4
#define FILE_MENU_FORMAT_COL 1
#define FILE_MENU_FORMAT_SIZE_ROW 4
#define FILE_MENU_FORMAT_SIZE_COL 2

typedef struct _FILE_PANEL {
    WINDOW* panel;
    char* current_directory;
    MENU *file_menu;
    WINDOW* menu_sub_win;
} File_Panel;

bool init_file_panel(File_Panel** file_panel, int num);
bool resize_file_panel(File_Panel* file_panel, int num);
void refresh_file_panel(File_Panel *file_panel);
ITEM** load_dir(File_Panel* file_panel);
bool init_file_menu(File_Panel* file_panel, ITEM** items) ;

