#include "winmanip.h"

void clear_y_str(WINDOW* win, int y, int start_x, int len) {
    int size = start_x + len;
    if(len == 0) {
        size = getmaxx(win);
    } else if (len < 0) {
        size = getmaxx(win) + len;
    }

    for(int i = start_x; i < size; i++) {
        mvwaddch(win, y, i, ' ');
    } 
}

void ext_start_color() {
    start_color();

    init_color(COLOR_VIOLET, 94, 9, 180);
    init_color(COLOR_DEEP_BLUE, 2, 46, 112);
    init_color(COLOR_BEIGE, 209, 188, 138);
    init_color(COLOR_GRAY, 255, 255, 255);
    
    init_pair(MENU_GREEN, COLOR_GREEN, COLOR_DEEP_BLUE);
    init_pair(MENU_YELLOW, COLOR_YELLOW, COLOR_DEEP_BLUE);
    init_pair(MENU_RED, COLOR_RED, COLOR_DEEP_BLUE);
    init_pair(MENU_GRAY, COLOR_GRAY, COLOR_DEEP_BLUE);
    init_pair(MENU_WHITE, COLOR_WHITE, COLOR_DEEP_BLUE);
    init_pair(MENU_BLUE, COLOR_BLUE, COLOR_DEEP_BLUE);
    init_pair(MENU_BEIGE, COLOR_BEIGE, COLOR_DEEP_BLUE);
    init_pair(MENU_SLCTD_ITEM, COLOR_RED, COLOR_CYAN);

	init_pair(TOP_PANEL_COLOR, COLOR_WHITE, COLOR_VIOLET);
	init_pair(SLCTD_TOP_PANEL, COLOR_DEEP_BLUE, COLOR_WHITE);

	init_pair(BOTTOM_PANEL_ITEM, COLOR_WHITE, COLOR_BLACK);
    init_pair(BOTTOM_PANEL_SLCTD_ITEM, COLOR_BLACK, COLOR_CYAN);
    
    init_pair(EXCEPTION_COLOR, COLOR_WHITE, COLOR_RED);
    init_pair(WARNING_BOX_COLOR, COLOR_WHITE, COLOR_YELLOW);

    init_pair(SLCTD_EXCEPTION_COLOR, COLOR_BLACK, COLOR_WHITE);
}

void mvwaddwstr_color(WINDOW* win, int y, int x, wchar_t* wstring, short color_pair) {
    cchar_t buffer;
    size_t size = wcslen(wstring);

    for(int i = 0; i < size; i++) {
        setcchar(&buffer, wstring + i, A_NORMAL, color_pair, NULL);
        mvwadd_wch(win, y, x + i, &buffer);
    }

}

void mvwaddnwstr_color(WINDOW* win, int y, int x, wchar_t* wstring, short color_pair, int n) {
    cchar_t buffer;
    size_t size = n;

    for(int i = 0; i < size; i++) {
        setcchar(&buffer, wstring + i, A_NORMAL, color_pair, NULL);
        mvwadd_wch(win, y, x + i, &buffer);
    }

}

void recolor_str(WINDOW* win, int y, short color_pair) {
    int cols_len = getmaxx(win);
    wchar_t *wstr = (wchar_t *)calloc(cols_len + 1, sizeof(wchar_t));
    mvwinnwstr(win, y, 0, wstr, cols_len);

    cchar_t buffer;
    for(int i = 0; i < cols_len; i++) {
        setcchar(&buffer, wstr + i, A_NORMAL, color_pair, NULL);
        mvwadd_wch(win, y, i, &buffer);
    }
}