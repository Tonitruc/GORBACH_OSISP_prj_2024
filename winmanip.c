#include "winmanip.h"

void clear_y_str(WINDOW* win, int y, int x) {
    for(int i = x; i < getmaxx(win); i++) {
        mvwprintw(win, y, i, " ");
    } 
}

void ext_start_color() {
    start_color();

    init_color(COLOR_VIOLET, 94, 9, 180);
    init_color(COLOR_DEEP_BLUE, 2, 46, 112);
    init_color(COLOR_BEIGE, 209, 188, 138);
    init_color(COLOR_GRAY, 120, 255, 255);
    
    init_pair(MENU_GREEN, COLOR_GREEN, COLOR_DEEP_BLUE);
    init_pair(MENU_YELLOW, COLOR_YELLOW, COLOR_DEEP_BLUE);
    init_pair(MENU_RED, COLOR_RED, COLOR_DEEP_BLUE);
    init_pair(MENU_GRAY, COLOR_GRAY, COLOR_DEEP_BLUE);
    init_pair(MENU_WHITE, COLOR_WHITE, COLOR_DEEP_BLUE);
    init_pair(MENU_BLUE, COLOR_BLUE, COLOR_DEEP_BLUE);
    init_pair(MENU_BEIGE, COLOR_BEIGE, COLOR_DEEP_BLUE);
}

void mvwaddwstr_color(WINDOW* win, int y, int x, wchar_t* wstring, short color_pair) {
    cchar_t buffer;
    attr_t attrs;
    short clr_pair;

    size_t size = wcslen(wstring);

    for(int i = 0; i < size; i++) {
        setcchar(&buffer, wstring + i, A_NORMAL, color_pair, NULL);
        mvwadd_wch(win, y, x + i, &buffer);
    }

}