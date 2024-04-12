#pragma once

#define COLOR_VIOLET 9
#define COLOR_DEEP_BLUE 10
#define COLOR_ORANGE 11
#define COLOR_DEEP_PINK 12
#define COLOR_GRAY 13
#define COLOR_BEIGE 14

#define MENU_GREEN 30
#define MENU_YELLOW 31
#define MENU_BLUE 32
#define MENU_RED 33
#define MENU_GRAY 34
#define MENU_WHITE 35
#define MENU_BEIGE 36

#include <ncursesw/ncurses.h>
#include <wchar.h>

void clear_y_str(WINDOW* win, int y, int x);
void ext_start_color();
void mvwaddwstr_color(WINDOW* win, int y, int x, wchar_t* wstring, short color_pair);