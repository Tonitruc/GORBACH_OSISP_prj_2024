#pragma once

//____________ Библиотека для меню для работы со списком данных________________

#include <ncurses.h>
#include <wchar.h>
#include <stdbool.h>
#include "fwchar.h"

#define BASE_ROW_AMOUNT 1
#define BASE_COL_AMOUNT 1
#define BASE_SELECTED_ITEM 0

#define BASE_SELECT_SYM '>'
#define BASE_POINT_SYM '-'
#define BASE_SEPARATE_SYM L'\u2502'
#define U_HLINE L'\u2500'
//Вывод строки реверсивного цвета
#define mvwprintwr(win, y, x, format_str, ...) wattron(win, A_REVERSE); mvwprintw(win, y, x, format_str, __VA_ARGS__); wattroff(win, A_REVERSE)

#define STNDRT_SETTINGS_GRID(set_grid) set_grid = SPRT_INTERMEDIATE | NON_DESIG_ITEMS | NON_COL_SIZE | NON_COL_NAME | ALLIGMENT_LEFT
#define STNDRT_SETTINGS_F_HBOX(set_f_hbox) set_f_hbox = SPRT_INTERMEDIATE | DESIG_ITEMS | USER_COL_SIZE | USE_COL_NAME | ALLIGMENT_LEFT
//Указатель на функцию для выполнения действия при нажатии на элемент меню
typedef void (*MIACTION)();

typedef struct _MITEM {
    wchar_t* string;
    int y; int x;
    int col;
    size_t size;
    MIACTION action;
    short color;
    chtype pnt_sym; short pnt_color;
    short slct_color; short sprt_color;
} MITEM;
//Тип меню
typedef enum _TYPE_MENU {
    GRID = 0,
    F_HBOX = 1 
} TYPE_MENU;
//Настройки меню
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
    ALLIGMENT_CENTER = 1024,
    ALLIGMENT_LEFT = 2048
} SETTINGS_MENU;

typedef enum _DIRECTRION_SORT {
    INCREASING, 
    DECREASING
} SORT_DIR;
//Функция для сокращения выводимых данных
typedef wchar_t* (*ABREVIATED)(wchar_t*, int);

typedef struct MENU {
    TYPE_MENU type;
    WINDOW* parwin;
    WINDOW* subwin;

    MITEM** items; 
    size_t size;
    int select;
    MITEM* iselect;

    MIACTION items_action;
    ABREVIATED* items_abr;
    short slctd_item_color_pair;

    int max_rows; int max_column;
    bool div_static_size;
    int columns; int rows;
    double* columns_size;
    bool is_sort;
    int sorted_col;
    SORT_DIR direction_sort;

    wchar_t sprt_sym;
    chtype slct_sym;
    chtype pnt_sym;

    MITEM** column_names;
    chtype color_front_col;
    SETTINGS_MENU set_menu;

    MITEM** group;
    short group_color;
} MENU;
//Ответ после работы функций позиционирования в меню
typedef enum _REQ_KEY {
    NON_REQ = 0,
    REQ_DOWN_ITEM = 1,
    REQ_UP_ITEM = 2,
    REQ_LEFT_ITEM = 4,
    REQ_RIGHT_ITEM = 8,
    ITEM_CLICKED = 16,
    COLUMN_CLICKED = 32,
    GROUP_ACT = 64,
    SAME_ITEM = 128,
} REQ_KEY;

#define verify_menu(window) ({  \
    WINDOW* menu_win = derwin(window, 1, getmaxx(window) - 4, getmaxy(window) - 1, 2); \
    keypad(menu_win, TRUE); \
    MITEM **items = (MITEM**)calloc(3, sizeof(MITEM*)); \
    items[0] = init_menu_item(L"[ ОК ]"); \
    items[1] = init_menu_item(L"[ ОТМЕНА ]"); \
    SETTINGS_MENU set_menu = NONE_SPRT | NON_DESIG_ITEMS | USER_COL_SIZE | NON_COL_NAME | ALLIGMENT_CENTER; \
    MENU* menu = init_menu(items, window, menu_win, GRID, set_menu); \
    init_menu_format(menu, 1, 2); \
    menu->slctd_item_color_pair = SLCTD_EXCEPTION_COLOR; \
    set_columns_size(menu, (double*)2, 0.5, 0.5); \
    menu; })

//-------------Элементы меню-------------

MITEM* init_menu_item(const wchar_t* string);
size_t size_items(MITEM** items);
int find_mitem(MITEM** items, wchar_t* string);

//-------------Настройка меню-------------

MENU* init_menu(MITEM** items, WINDOW* p_win, WINDOW* sub_win, TYPE_MENU type, SETTINGS_MENU set_menu);
bool init_menu_win(MENU* menu, WINDOW* pwin); 
bool init_menu_sub(MENU* menu, WINDOW* subwin); 
bool init_menu_format(MENU* menu, int row, int column); 
void set_columns_size(MENU *menu, double* col_len, ...);
void set_sprt_sym(MENU* menu, wchar_t sprt_sym);
void set_slct_sym(MENU* menu, chtype slct_sym);
void set_pnt_sym(MENU* menu, chtype pnt_sym);
void set_column_name(MENU *menu, const wchar_t* names, ...);
void calc_item_coord(MENU* menu);
void set_item_action(MENU* menu, int num, MIACTION action);
void init_color_slctd_item(MENU* menu, short color_pair);

//free menu
void set_new_items(MENU* menu, MITEM** new_items, int select);
void add_item(MENU* menu, MITEM* new_item, int index);
void delete_item(MENU* menu, int index);
void free_menu(MENU *menu);

//-------------Вывод меню-------------
void print_menu(MENU* menu);
void unprint_menu(MENU* menu);
void menu_driver(MENU* menu, REQ_KEY key);
void resize_menu(MENU* menu);
void offset_y_items(MENU* menu, int y);

//Обработка нажатий  
REQ_KEY find_click_item(MENU* menu, MEVENT event);
void slctd_item_action(MENU* menu);
int clicked_column(MENU* menu, MEVENT event);
void change_menu_action(MENU* menu, REQ_KEY key, int prev_selected_item);

//Работы с группой элементов 
int mslct(MENU* menu);
bool mcontains(MENU* menu, const wchar_t* string);
bool is_clear_group(MENU* menu);
int* get_group_pnt(MENU* menu);
int group_size(MENU* menu);
void clear_group(MENU* menu);

wchar_t* standart_abreviated(wchar_t* item, int col_size);