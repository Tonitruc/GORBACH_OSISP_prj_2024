#pragma once

#include <stdbool.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>

#include "menu.h"
#include "file_types.h"
#include "fwchar.h"
#include "winmanip.h"
#include "file_operation.h"
#include "message_box.h"
#include "text_box.h"
#include "toggle_button.h"

#define START_DIR L"/home/tonitrus"
#define DIR_RETURN L".."
#define DIR_RETURN_NAME L"-ВВЕРХ-"

#define COLS_PERCENT 0.50
#define ROWS_PERCENT 0.10

#define FILE_PANEL_SIZE(height, width) height = (LINES - 2); width = (COLS - COLS_PERCENT * COLS)
#define PANEL_START_POS(starty, startx, num) starty = 1; startx = 0 + num * COLS * COLS_PERCENT

#define FILE_MENU_START_ROW 4
#define FILE_MENU_START_COL 1
#define FILE_MENU_FORMAT_ROW 4
#define FILE_MENU_FORMAT_COL 3
#define FILE_MENU_FORMAT_SIZE_ROW 4
#define FILE_MENU_FORMAT_SIZE_COL 2
#define TAB_LIST_ROW 3
#define TAB_LIST_COL 1

#define SPACE_SELECTED_ELEMENT_ROW 5
#define SELECTED_FILE_SPACE_SIZE(panel, height, width) width = getmaxx(panel) - 1; height = getmaxy(panel) - SPACE_SELECTED_ELEMENT_ROW;
#define CURRENT_FILE_POS(panel, height, width) SELECTED_FILE_SPACE_SIZE(panel, height, width); width = 3; height += 2
#define CURRENT_FILE_LOG L"<-ТЕКУЩИЙ ФАЙЛ"

#define TAB_SPACE_ROW 2
#define TAB_SPACE_SIZE(panel, height, width) height = TAB_SPACE_ROW; width = getmaxx(panel) - 1
#define TAB_SPACE_POS(row, col) row = 1; col = 1; 
#define TAB_NAME L"<-ВКЛАДКА"

#define CUR_FILE_SIZE(win) ({ \
    int size = getmaxx(win) - 4; \
    size; \
})

#define MAX_TABS 10

typedef struct _TAB {
    wchar_t* directory;
    int select;
} TAB;


typedef struct _FILE_PANEL {
    WINDOW* panel;
    wchar_t* current_directory;
    MENU *file_menu;
    WINDOW* menu_sub_win;
    WINDOW* parent_window;
    LIST* files_info;
    SORT_DIR sort_dir;

    TAB tabs[MAX_TABS];
    MENU* tabs_panel;

    MENU* tabs_menu;
    int amount_tabs;

    LIST* group;
} FILE_PANEL;

FILE_PANEL* init_file_panel(WINDOW* parent_window, int num);
bool resize_file_panel(FILE_PANEL* file_panel, int num);
void refresh_file_panel(FILE_PANEL *file_panel);
MITEM** load_dir(FILE_PANEL* file_panel);
bool init_file_menu(FILE_PANEL* file_panel, MITEM** items);
void print_selected_file_space(FILE_PANEL* file_panel);
bool mouse_event_handler(FILE_PANEL *file_panel, MEVENT mevent);
bool keyboard_event_handler(FILE_PANEL *file_panel, int key);
void print_current_file(FILE_PANEL *file_panel, bool with_clear);
void print_tab_space(FILE_PANEL *file_panel);
void print_current_directory(FILE_PANEL *file_panel, bool with_clear);
int dialog_win_cm(FILE_PANEL* file_panel, wchar_t* title);

//file CRUD
int del_file(FILE_PANEL* file_panel);
int create_dir(FILE_PANEL* file_panel);
bool correction_file(FILE_PANEL* file_panel);
bool open_file(FILE_PANEL* file_panel);
int rename_dir(FILE_PANEL* file_panel);

//free memory
void free_file_panel(FILE_PANEL* file_panel);

//tabs
bool delete_tab(FILE_PANEL* file_panel);
bool save_tab(FILE_PANEL* file_panel);
void load_tab(FILE_PANEL* file_panel, int prev_tab_num);
void print_current_tab(FILE_PANEL* file_panel);

MITEM** init_files(LIST* list);

bool create_sym_link(FILE_PANEL* file_panel);
bool copy_files(FILE_PANEL* file_panel, FILE_PANEL* dep);