

#include <panel.h>

#define INIT_BASE_WIN init_pair(1, COLOR_WHITE, COLOR_WHITE); attron(COLOR_PAIR(1)); \
					wbkgd(stdscr, COLOR_PAIR(1)); refresh();  //Set the base style for the main window

int main() {
	initscr();
	start_color();
	INIT_BASE_WIN

	getch();
	endwin();
}