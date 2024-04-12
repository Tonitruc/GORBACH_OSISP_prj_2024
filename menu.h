#pragma once

#include <ncursesw/ncurses.h>
#include <wchar.h>
#include <stdbool.h>

#define BASE_ROW_AMOUNT 1
#define BASE_COL_AMOUNT 1
#define BASE_SELECTED_ITEM 0

#define BASE_SELECT_SYM '>'
#define BASE_POINT_SYM '-'
#define BASE_SEPARATE_SYM L'\u2502'
#define U_HLINE L'\u2500'

#define COLOR_PAIR_MENU 20
#define STNDRT_COLOR_MENU_PAIR 21

#define mvwprintwr(win, y, x, format_str, ...) wattron(win, A_REVERSE); mvwprintw(win, y, x, format_str, __VA_ARGS__); wattroff(win, A_REVERSE)

#define STNDRT_SETTINGS_GRID(set_grid) set_grid = SPRT_INTERMEDIATE | NON_DESIG_ITEMS | NON_COL_SIZE | NON_COL_NAME
#define STNDRT_SETTINGS_F_HBOX(set_f_hbox) set_f_hbox = SPRT_INTERMEDIATE | DESIG_ITEMS | USER_COL_SIZE | USE_COL_NAME

typedef void (*MIACTION)();

typedef struct _MITEM {
    wchar_t* string;
    int y; int x;
    size_t size;
    MIACTION action;
    short color;
    chtype pnt_sym;
} MITEM;

typedef enum _TYPE_MENU {
    GRID = 0,
    F_HBOX = 1 
} TYPE_MENU;

typedef enum _SETTINGS_MENU {
    //Separate symbol
    NONE_SPRT = 1,
    SPRT_ALL = 2,
    SPRT_INTERMEDIATE = 4,
    //desing items
    NON_DESIG_ITEMS = 8,
    DESIG_ITEMS = 16,
    //col size
    STNDRT_COL_SIZE = 32,
    USER_COL_SIZE = 64,
    NON_COL_SIZE = 128, 
    //col name
    USE_COL_NAME = 256,
    NON_COL_NAME = 512,

} SETTINGS_MENU;

typedef struct MENU {
    TYPE_MENU type_menu;
    WINDOW* parent_window;
    WINDOW* sub_window;

    MITEM** items; 
    size_t amount_items;
    int selected_item;
    MIACTION items_action;
    short slctd_item_color_pair;

    int max_rows; 
    int columns; int rows;
    double* columns_size;

    wchar_t sprt_sym;
    chtype slct_sym;
    chtype pnt_sym;

    MITEM** column_names;
    chtype color_front_col;
    SETTINGS_MENU set_menu;
} MENU;

typedef enum _REQ_KEY {
    NON_REQ,
    REQ_DOWN_ITEM,
    REQ_UP_ITEM,
    REQ_LEFT_ITEM,
    REQ_RIGHT_ITEM
} REQ_KEY;

//-------------Menu items-------------

MITEM* init_menu_item(const wchar_t* string);
size_t size_items(MITEM** items);

//-------------Menu Settings-------------

//new
MENU* init_menu(MITEM** items, WINDOW* p_win, WINDOW* sub_win, TYPE_MENU type, SETTINGS_MENU set_menu);
bool init_menu_win(MENU* menu, WINDOW* pwin); //set parent win for menu
bool init_menu_sub(MENU* menu, WINDOW* subwin); //Set subwin for menu
bool init_menu_format(MENU* menu, int row, int column); //Amount of row and column
void set_columns_size(MENU *menu, double* col_len, ...);
void set_sprt_sym(MENU* menu, wchar_t sprt_sym);
void set_slct_sym(MENU* menu, chtype slct_sym);
void set_pnt_sym(MENU* menu, chtype pnt_sym);
void set_column_name(MENU *menu, const wchar_t* names, ...);
void calc_item_coord(MENU* menu);
void set_item_action(MENU* menu, int num, MIACTION action);
void init_color_slctd_item(MENU* menu, short color_pair);

//free menu
void set_new_items(MENU* menu, MITEM** new_items);
void free_menu(MENU *menu);

//-------------Print Menu-------------

void print_menu(MENU* menu);
void unprint_menu(MENU* menu);
void menu_driver(MENU* menu, REQ_KEY key);
bool resize_menu(MENU* menu);
void menu_str_slctd_color(MENU* win, int y_str, bool is_reversed);
void offset_y_items(MENU* menu, int y);
void change_menu_action(MENU* menu, REQ_KEY key, int prev_selected_item);

//Mouse events 
bool find_click_item(MENU* menu, MEVENT event);
void slctd_item_action(MENU* menu);