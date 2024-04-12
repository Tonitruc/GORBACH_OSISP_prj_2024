#include <panel.h>
#include "file_panel.h"
#include <signal.h>
#include <locale.h>
#include <stdbool.h>
#include "file_types.h"
#include <stdlib.h>
#include "menu.h"

#define MIN_HEIGHT 15
#define MIN_WIDTH 15

#define AMOUNT_OF_FILE_PANEL 2

File_Panel* first_file_panel;
File_Panel* second_file_panel;
MENU* bottom_panel_val;
WINDOW* top_panel_win;


PANEL* application[2];

#define INIT_BASE_WIN init_pair(1, COLOR_WHITE, COLOR_DEEP_BLUE); attron(COLOR_PAIR(1)); \
					wbkgd(stdscr, COLOR_PAIR(1)); refresh();  //Set the base style for the main window
#define BOTTOM_PANEL_SIZE(height, width) height = 1; width = getmaxx(stdscr)


const wchar_t* top_panel_points[] = {
	L" Левая панель ",
	L" Настройка ",
	L" Команда ",
	L" Файл ",
	L" Правая панель ",
};

bool resize_app();
bool init_app();
void close_app();
void refresh_app();

int main() {
	init_app();
	refresh();
	bool is_break = false;
	mousemask(ALL_MOUSE_EVENTS, NULL);

	while(1) {
		refresh();
		wrefresh(first_file_panel->menu_sub_win);
		int key;
 		key = getch();
		
		if(key == KEY_RESIZE) {
			resize_app();
		}
		else if (key == KEY_F(1)) {
			is_break = true;
			close_app();
		}
		else if (key == KEY_LEFT) {
			menu_driver(bottom_panel_val, REQ_LEFT_ITEM);
		}
		else if (key == KEY_RIGHT) {
			menu_driver(bottom_panel_val, REQ_RIGHT_ITEM);
		}
		else if (key == 'm') {
			
		}
		else {
			event_handler(first_file_panel, key);
		}

		if(is_break)
			break;
	}

	return 1;
	} 


bool init_app() {
	setlocale(LC_ALL, "");
	initscr();

	cbreak();
 	noecho();
 	keypad(stdscr, TRUE);
	curs_set(false);
	mouseinterval(3); 

	ext_start_color();

	INIT_BASE_WIN

	MITEM** items = (MITEM**)calloc(6, sizeof(MITEM*));
	for(int i = 0; i < 5; i++) {
		items[i] = init_menu_item(top_panel_points[i]);
	}
	int width, height;
	BOTTOM_PANEL_SIZE(height, width);
	WINDOW* par = newwin(height, width, 0, 0);
	init_pair(3, COLOR_WHITE, COLOR_VIOLET);
	init_pair(5, COLOR_DEEP_BLUE, COLOR_WHITE);
	init_pair(4, COLOR_RED, COLOR_CYAN);
	init_pair(6, COLOR_RED, COLOR_DEEP_BLUE);
	wattron(par, COLOR_PAIR(3));
	wbkgd(par, COLOR_PAIR(3));
	SETTINGS_MENU set_menu = SPRT_INTERMEDIATE | NON_DESIG_ITEMS | USER_COL_SIZE | NON_COL_NAME;
	bottom_panel_val = init_menu(items, par, derwin(par, height, width / 2, 0, 0), GRID, set_menu);
	set_sprt_sym(bottom_panel_val, L' ');
	init_color_slctd_item(bottom_panel_val, 5);
	init_menu_format(bottom_panel_val, 1, 5);
	set_columns_size(bottom_panel_val, (double*)5, 0.22, 0.22, 0.22, 0.12, 0.22);
	print_menu(bottom_panel_val);
	wrefresh(bottom_panel_val->parent_window);

	if(!init_file_panel(&first_file_panel, stdscr, 0) || !init_file_panel(&second_file_panel, stdscr, 1))
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

    //werase(bottom_panel_val->parent_window);
    //wresize(bottom_panel_val->parent_window, 1, getmaxx(stdscr));
    //wresize(bottom_panel_val->sub_window,1, getmaxx(stdscr) / 2);

    //resize_menu(bottom_panel_val);
	//wrefresh(bottom_panel_val->parent_window);

	return true;
}

void close_app() {
	free(first_file_panel);
	free(second_file_panel);
	endwin();
}

void refresh_app() {
	
}