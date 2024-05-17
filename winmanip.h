#pragma once

#include <ncurses.h>
#include <wchar.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define COLOR_VIOLET 9
#define COLOR_DEEP_BLUE 10
#define COLOR_ORANGE 11
#define COLOR_DEEP_PINK 12
#define COLOR_GRAY 13
#define COLOR_BEIGE 14
#define COLOR_LIGHT_YELLOW 15
#define COLOR_LIGHT_GREEN 16
#define COLOR_DARK_BLUE 17
#define COLOR_OLIVE 18
#define COLOR_LIGHT_RED 19
#define COLOR_SKY 20

#define MENU_GREEN 30
#define MENU_OLIVE 31
#define MENU_BLUE 32
#define MENU_RED 33
#define MENU_GRAY 34
#define MENU_WHITE 35
#define MENU_BEIGE 36
#define MENU_ORANGE 60
#define MENU_LIGHT_RED 61

#define MENU_SLCTD_ITEM 37

#define TOP_PANEL_COLOR 38
#define SLCTD_TOP_PANEL 39

#define BOTTOM_PANEL_ITEM 40
#define BOTTOM_PANEL_SLCTD_ITEM 41

#define EXCEPTION_COLOR 42
#define SLCTD_EXCEPTION_COLOR 43
#define WARNING_BOX_COLOR 44
#define MENU_TAB_PANEL 45
#define MENU_GROUP 46

#define U_ARROW_UP L"\u2191"
#define U_ARROW_DOWN L"\u2193"

#define CENTER_Y (LINES - 1) / 2
#define CENTER_X (COLS - 1) / 2
#define CENTER_SCR(start_y, start_x) start_x = CENTER_X; start_y = CENTER_Y

void clear_y_str(WINDOW* win, int y, int start_x, int len);
void ext_start_color();
void mvwaddwstr_color(WINDOW* win, int y, int x, wchar_t* wstring, short color_pair);
void mvwaddnwstr_color(WINDOW* win, int y, int x, wchar_t* wstring, short color_pair, int n);
void recolor_str(WINDOW* win, int y, short color_pair);
int start_app(const char* path, const char* args[]);

#define cwbcrt_win(height, width, color_pair, title) crt_box_win(height, width, CENTER_Y - height / 2, CENTER_X - width / 2, color_pair, title);
#define wbrt_derwin(tab_win, color_pair) crt_derwin(tab_win, 1, 1, color_pair);

WINDOW* crt_box_win(int height, int width, int y, int x, short color_pair, wchar_t* title);
WINDOW* crt_derwin(WINDOW* window, int y, int x, short color_pair);