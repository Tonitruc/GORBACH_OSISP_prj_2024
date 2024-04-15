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

MENU* upper_panel;
WINDOW* top_panel_menu;

MENU* lower_panel;

PANEL* application[2];

#define INIT_BASE_WIN init_pair(1, COLOR_WHITE, COLOR_DEEP_BLUE); attron(COLOR_PAIR(1)); \
					wbkgd(stdscr, COLOR_PAIR(1)); refresh();  //Set the base style for the main window
#define TOP_PANEL_SIZE(height, width) height = 1; width = getmaxx(stdscr)


#define AMOUNT_TOP_PANEL_ITEMS 5

const wchar_t* upper_panel_points[] = {
	L" Левая панель ",
	L" Настройка ",
	L" Команда ",
	L" Файл ",
	L" Правая панель ",
};

#define AMOUNT_LOWER_PANEL_ITEMS 9

const wchar_t* lower_panel_points[] = {
	L" Помощь",
	L" Правка",
	L" Просмотр",
	L" Правка",
	L" Копия",
	L" Каталог",
	L" Новый файл",
	L" Удалить",
	L" Выход",
};

bool resize_app();
bool init_app();
void close_app();
void refresh_app();

void init_lower_panel();
void init_upper_panel();

int main() {
	init_app();
	refresh();
	bool is_break = false;
	mousemask(ALL_MOUSE_EVENTS, NULL);
	MEVENT event;

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
			menu_driver(upper_panel, REQ_LEFT_ITEM);
		}
		else if (key == KEY_RIGHT) {
			menu_driver(upper_panel, REQ_RIGHT_ITEM);
		}
		else if (key == 'm') {
			show_panel(application[0]);
			update_panels();
			doupdate();
		}
		else if(key == 'n') {
			hide_panel(application[0]);
			update_panels();
			doupdate();
		} 
		else{
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
	mouseinterval(1); 

	ext_start_color();

	INIT_BASE_WIN

	if(!init_file_panel(&first_file_panel, stdscr, 0) || !init_file_panel(&second_file_panel, stdscr, 1))
		return false;

	refresh_file_panel(first_file_panel);
	refresh_file_panel(second_file_panel); 
	refresh();

	init_lower_panel();
	init_upper_panel();
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

void init_upper_panel() {
	MITEM** items = (MITEM**)calloc(AMOUNT_TOP_PANEL_ITEMS + 1, sizeof(MITEM*));
	for(int i = 0; i < AMOUNT_TOP_PANEL_ITEMS; i++) {
		items[i] = init_menu_item(upper_panel_points[i]);
	}

	int width, height;
	TOP_PANEL_SIZE(height, width);
	WINDOW* parent = newwin(height, width, 0, 0);
	wattron(parent, COLOR_PAIR(TOP_PANEL_COLOR));
	wbkgd(parent, COLOR_PAIR(TOP_PANEL_COLOR));
	WINDOW* sub = derwin(parent, height, width / 2, 0, 0);

	SETTINGS_MENU set_menu = SPRT_INTERMEDIATE | NON_DESIG_ITEMS | USER_COL_SIZE | NON_COL_NAME;
	upper_panel = init_menu(items, parent, sub, GRID, set_menu);

	set_sprt_sym(upper_panel, L' ');
	init_color_slctd_item(upper_panel, SLCTD_TOP_PANEL);
	init_menu_format(upper_panel, 1, AMOUNT_TOP_PANEL_ITEMS);
	set_columns_size(upper_panel, (double*)5, 0.22, 0.22, 0.22, 0.12, 0.22);

	print_menu(upper_panel);
	wrefresh(parent);
	//application[0] = new_panel(application);
}

void init_lower_panel() {
	MITEM** items = (MITEM**)calloc(AMOUNT_LOWER_PANEL_ITEMS + 1, sizeof(MITEM*));
	chtype num = '1';
	for(int i = 0; i < AMOUNT_LOWER_PANEL_ITEMS; i++) {
		items[i] = init_menu_item(lower_panel_points[i]);
		items[i]->pnt_sym = num++;
		items[i]->color = BOTTOM_PANEL_ITEM;
	}

	int width, height;
	TOP_PANEL_SIZE(height, width);
	WINDOW* parent = newwin(height, width, LINES - 1, 0);
	wattron(parent, COLOR_PAIR(BOTTOM_PANEL_ITEM));
	wbkgd(parent, COLOR_PAIR(MENU_WHITE));
	WINDOW* sub = derwin(parent, height, width, 0, 0);
	wattron(sub, COLOR_PAIR(BOTTOM_PANEL_ITEM));

	SETTINGS_MENU set_menu = NONE_SPRT | DESIG_ITEMS | STNDRT_COL_SIZE | NON_COL_NAME;
	lower_panel = init_menu(items, parent, sub, GRID, set_menu);
	init_menu_format(lower_panel, 1, AMOUNT_LOWER_PANEL_ITEMS);

	init_color_slctd_item(lower_panel, BOTTOM_PANEL_ITEM);

	print_menu(lower_panel);
	wrefresh(lower_panel->parent_window);
}