#pragma once

#include <stdbool.h>
#include <ncursesw/ncurses.h>
#include "menu.h"
#include "file_types.h"
#include "fwchar.h"
#include "winmanip.h"

#define START_DIR L"/home/tonitrus"
#define DIR_RETURN L".."
#define DIR_RETURN_NAME L"-ВВЕРХ-"

#define COLS_PERCENT 0.50
#define ROWS_PERCENT 0.10

#define FILE_PANEL_SIZE(height, width) height = (LINES - 2); width = (COLS - COLS_PERCENT * COLS)
#define PANEL_START_POS(starty, startx, num) starty = 1; startx = 0 + num * COLS * COLS_PERCENT

#define FILE_MENU_START_ROW 3
#define FILE_MENU_START_COL 1
#define FILE_MENU_FORMAT_ROW 4
#define FILE_MENU_FORMAT_COL 3
#define FILE_MENU_FORMAT_SIZE_ROW 4
#define FILE_MENU_FORMAT_SIZE_COL 2

#define SPACE_SELECTED_ELEMENT_ROW 5
#define SELECTED_FILE_SPACE_SIZE(panel, height, width) width = getmaxx(panel) - 1; height = getmaxy(panel) - SPACE_SELECTED_ELEMENT_ROW;
#define CURRENT_FILE_POS(panel, height, width) SELECTED_FILE_SPACE_SIZE(panel, height, width); width = 3; height += 2
#define CURRENT_FILE_LOG L"<-ТЕКУЩИЙ ФАЙЛ"

#define TAB_SPACE_ROW 2
#define TAB_SPACE_SIZE(panel, height, width) height = TAB_SPACE_ROW; width = getmaxx(panel) - 1
#define TAB_SPACE_POS(row, col) row = 1; col = 1; 
#define TAB_NAME L"<-ВКЛАДКА"

typedef struct _FILE_PANEL {
    WINDOW* panel;
    wchar_t* current_directory;
    MENU *file_menu;
    WINDOW* menu_sub_win;
    WINDOW* parent_window;
} File_Panel;

bool init_file_panel(File_Panel** file_panel, WINDOW* parent_window, int num);
bool resize_file_panel(File_Panel* file_panel, int num);
void refresh_file_panel(File_Panel *file_panel);
MITEM** load_dir(File_Panel* file_panel);
bool init_file_menu(File_Panel* file_panel, MITEM** items);
void print_selected_file_space(File_Panel* file_panel);
bool event_handler(File_Panel* file_panel, int key);
void print_current_file(File_Panel *file_panel, bool with_clear);
void print_tab_space(File_Panel *file_panel);
void print_current_directory(File_Panel *file_panel, bool with_clear);

