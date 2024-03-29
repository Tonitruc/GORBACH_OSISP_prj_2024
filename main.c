#include <panel.h>
#include "FILE_PANEL.h"
#include <signal.h>
#include <locale.h>
#include <stdbool.h>

#define MIN_HEIGHT 15
#define MIN_WIDTH 15

#define AMOUNT_OF_FILE_PANEL 2

FILE_PANEL* first_file_panel;
FILE_PANEL* second_file_panel;

#define INIT_BASE_WIN init_pair(1, COLOR_BLACK, COLOR_WHITE); attron(COLOR_PAIR(1)); \
					wbkgd(stdscr, COLOR_PAIR(1)); refresh();  //Set the base style for the main window

bool resize_app();
bool init_app();

int main() {
	init_app();

	while(1) {
		if(wgetch(stdscr) == KEY_RESIZE){
			resize_app();
		}
	}

	getch();
	endwin();
}


bool init_app() {
	setlocale(LC_ALL, "rus");
	initscr();
	curs_set(false);
	start_color();

	INIT_BASE_WIN
	if(!init_file_panel(&first_file_panel, 0) || !init_file_panel(&second_file_panel, 1))
		return false;

	show_file_panel(first_file_panel);
	show_file_panel(second_file_panel);
	refresh();

	return true;
}


bool resize_app() {
	clear();
	if(!resize_file_panel(first_file_panel, 0) || !resize_file_panel(second_file_panel, 1))
		return false;

	refresh();
	show_file_panel(first_file_panel);
	show_file_panel(second_file_panel);
	return true;
}