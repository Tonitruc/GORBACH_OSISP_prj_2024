#include <panel.h>
#include "file_panel.h"
#include <signal.h>
#include <locale.h>
#include <stdbool.h>
#include "file_types.h"

#define MIN_HEIGHT 15
#define MIN_WIDTH 15

#define AMOUNT_OF_FILE_PANEL 2

File_Panel* first_file_panel;
File_Panel* second_file_panel;

#define INIT_BASE_WIN init_pair(1, COLOR_WHITE, COLOR_BLACK); attron(COLOR_PAIR(1)); \
					wbkgd(stdscr, COLOR_PAIR(1)); refresh();  //Set the base style for the main window

bool resize_app();
bool init_app();


int main() {
	init_app();
	refresh();

	while(1) {

		switch (wgetch(first_file_panel->panel))
		{
		case KEY_DOWN:
			 menu_driver(first_file_panel->file_menu, REQ_DOWN_ITEM);
 		break;
 		case KEY_UP:
 			menu_driver(first_file_panel->file_menu, REQ_UP_ITEM);
 		break;
 		case KEY_NPAGE:
 			menu_driver(first_file_panel->file_menu, REQ_SCR_DPAGE);
 		break;
		case KEY_PPAGE:
 			menu_driver(first_file_panel->file_menu, REQ_SCR_UPAGE);
 		break;
 		case KEY_RESIZE: {
			resize_app();
		break;
		}
 	}
 		wrefresh(first_file_panel->panel);
 		refresh();
		}

	getch();
	endwin();
}


bool init_app() {
	setlocale(LC_ALL, "rus");
	initscr();
	cbreak();
 	noecho();
 	keypad(stdscr, TRUE);
	curs_set(false);
	start_color();

	INIT_BASE_WIN
	if(!init_file_panel(&first_file_panel, 0) || !init_file_panel(&second_file_panel, 1))
		return false;

	refresh_file_panel(first_file_panel);
	refresh_file_panel(second_file_panel); 
	refresh();

	return true;
}


bool resize_app() {
	clear();
	if(!resize_file_panel(first_file_panel, 0) || !resize_file_panel(second_file_panel, 1))
		return false;

	refresh();
	refresh_file_panel(first_file_panel);
	refresh_file_panel(second_file_panel);
	return true;
}