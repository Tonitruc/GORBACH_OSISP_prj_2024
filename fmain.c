#include "fmain.h"

WPANEL* wpanels[2];
WPANEL* wpanel;

//Menus for manage files
#define AMOUNT_TOP_PANEL_ITEMS 3
MENU* upper_panel;
WINDOW* setting_panel_window;

MENU* setting_panel_menu;

#define AMOUNT_SETTINGS 3

const wchar_t* setting_points[] = {
	L" Список файлов",
	L" Просмотр файла ",
	L" Информация о файле "
};

WINDOW* command_panel_window;
MENU* command_panel_menu;

#define AMOUNT_COMMAND 5

const wchar_t* command_points[] = {
	L" Символьная ссылка ",
	L" Архивация ",
	L" Разорхивация ",
	L" Без сортировки ",
	L" Поиск файла "
};

const wchar_t* upper_panel_points[] = {
	L" Левая панель ",
	L" Команда ",
	L" Правая панель ",
};

#define AMOUNT_LOWER_PANEL_ITEMS 9
MENU* lower_panel;

const wchar_t* lower_panel_points[] = {
	L"Файл",
	L"Правка",
	L"Просмотр",
	L"Перенос",
	L"Копия",
	L"Каталог",
	L"Переименовать",
	L"Удалить",
	L"Выход",
};

//Levels of windows 
PANEL* application[2];

bool init_app() {
	setlocale(LC_ALL, "ru_RU.UTF-8");

	initscr();
	cbreak();
 	noecho();
 	keypad(stdscr, TRUE);
	curs_set(false);
	mouseinterval(5);
	mousemask(ALL_MOUSE_EVENTS, NULL);					

	ext_start_color();

	INIT_BASE_WIN

	wpanels[0] = init_wpanel(init_file_panel(stdscr, 0));
	wpanels[1] = init_wpanel(init_file_panel(stdscr, 1));
	wpanels[0]->dep = wpanels[1];
	wpanels[1]->dep = wpanels[0];

	wpanel = wpanels[0];

	init_lower_panel();
	init_upper_panel();

	init_setting_menu();
	application[0] = new_panel(setting_panel_window);
	hide_panel(application[0]);

	init_command_menu();
	application[1] = new_panel(command_panel_window);
	hide_panel(application[1]);

	update_panels();
	doupdate();

	refresh();
	return true;
}


bool resize_app() {
    wresize(upper_panel->parwin, 1, getmaxx(stdscr));
    wresize(upper_panel->subwin,1, getmaxx(stdscr));

	werase(stdscr);
    resize_menu(upper_panel);

	werase(upper_panel->parwin);
	print_menu(upper_panel);

	resize_wpanel(wpanels[0], 0);
	resize_wpanel(wpanels[1], 1);

    wresize(lower_panel->parwin, 1, getmaxx(stdscr));
	mvderwin(lower_panel->parwin, LINES - 1, 0);
	mvwin(lower_panel->parwin, LINES - 1, 0);

    wresize(lower_panel->subwin, 1, getmaxx(stdscr));
	mvderwin(lower_panel->subwin, 0, 0);
	mvwin(lower_panel->subwin, LINES - 1, 0);

    resize_menu(lower_panel);
	werase(lower_panel->parwin);

	print_menu(lower_panel);

	return true;
}

void close_app() {

	free_menu(upper_panel);
	free_menu(lower_panel);
	
	delwin(setting_panel_window);
	delwin(command_panel_window);

	endwin();
}

void refresh_app() {
	wnoutrefresh(upper_panel->parwin);
	wnoutrefresh(upper_panel->subwin);

	refersh_wpanel(wpanels[0]);
	refersh_wpanel(wpanels[1]);

	wnoutrefresh(lower_panel->parwin);

	doupdate();
}

void init_upper_panel() {
	MITEM** items = (MITEM**)calloc(AMOUNT_TOP_PANEL_ITEMS + 1, sizeof(MITEM*));
	for(int i = 0; i < AMOUNT_TOP_PANEL_ITEMS; i++) {
		items[i] = init_menu_item(upper_panel_points[i]);
	}

	int width, height;
	TOP_PANEL_SIZE(height, width);
	WINDOW* parent = derwin(stdscr, height, width, 0, 0);
	wattron(parent, COLOR_PAIR(TOP_PANEL_COLOR));
	wbkgd(parent, COLOR_PAIR(TOP_PANEL_COLOR));
	WINDOW* sub = derwin(parent, height, COLS, 0, 0);

	SETTINGS_MENU set_menu = SPRT_INTERMEDIATE | NON_DESIG_ITEMS | USER_COL_SIZE | NON_COL_NAME;
	upper_panel = init_menu(items, parent, sub, GRID, set_menu);
	upper_panel->div_static_size = false;

	set_sprt_sym(upper_panel, L'|');
	init_color_slctd_item(upper_panel, SLCTD_TOP_PANEL);
	init_menu_format(upper_panel, 1, AMOUNT_TOP_PANEL_ITEMS);
	set_columns_size(upper_panel, (double*)3, 20.0, 17.0, 16.0);

	print_menu(upper_panel);
}

void init_lower_panel() {
	MITEM** items = (MITEM**)calloc(AMOUNT_LOWER_PANEL_ITEMS + 1, sizeof(MITEM*));

	chtype num = '1';
	for(int i = 0; i < AMOUNT_LOWER_PANEL_ITEMS; i++) {
		items[i] = init_menu_item(lower_panel_points[i]);
		items[i]->pnt_sym = num++;

		items[i]->color = BOTTOM_PANEL_SLCTD_ITEM;
		items[i]->pnt_color = BOTTOM_PANEL_ITEM;
		items[i]->slct_color = BOTTOM_PANEL_ITEM;
		items[i]->sprt_color = BOTTOM_PANEL_ITEM;
	}

	int width, height;
	TOP_PANEL_SIZE(height, width);
	WINDOW* parent = derwin(stdscr, height, width, LINES - 1, 0);
	wattron(parent, COLOR_PAIR(BOTTOM_PANEL_SLCTD_ITEM));
	wbkgd(parent, COLOR_PAIR(BOTTOM_PANEL_SLCTD_ITEM));
	WINDOW* sub = derwin(parent, height, width, 0, 0);
	wattron(sub, COLOR_PAIR(BOTTOM_PANEL_SLCTD_ITEM));

	SETTINGS_MENU set_menu = SPRT_INTERMEDIATE | DESIG_ITEMS | STNDRT_COL_SIZE | NON_COL_NAME;
	lower_panel = init_menu(items, parent, sub, GRID, set_menu);
	lower_panel->slct_sym = '1';
	lower_panel->sprt_sym = L' ';

	init_menu_format(lower_panel, 1, AMOUNT_LOWER_PANEL_ITEMS);
	init_color_slctd_item(lower_panel, BOTTOM_PANEL_SLCTD_ITEM);

	print_menu(lower_panel);
}

void init_command_menu() {
	MITEM** items = (MITEM**)calloc(AMOUNT_COMMAND + 1, sizeof(MITEM*));
	for(int i = 0; i < AMOUNT_COMMAND; i++) {
		items[i] = init_menu_item(command_points[i]);
	}

	command_panel_window = newwin(AMOUNT_COMMAND + 2, 40, 1, 20);
	box(command_panel_window, 0, 0);
	wattron(command_panel_window, COLOR_PAIR(MENU_SLCTD_ITEM));
	wbkgd(command_panel_window, COLOR_PAIR(MENU_SLCTD_ITEM));
	WINDOW* subwin = derwin(command_panel_window, AMOUNT_COMMAND, 38, 1, 1);
	SETTINGS_MENU set_menu = NONE_SPRT | NON_DESIG_ITEMS | NON_COL_SIZE | NON_COL_NAME;
	command_panel_menu = init_menu(items, command_panel_window, subwin, F_HBOX, set_menu);
	init_color_slctd_item(command_panel_menu, SLCTD_TOP_PANEL);
	init_menu_format(command_panel_menu, AMOUNT_COMMAND, 1);

	print_menu(command_panel_menu);
}

void init_setting_menu() {
	MITEM** items = (MITEM**)calloc(AMOUNT_SETTINGS + 1, sizeof(MITEM*));
	for(int i = 0; i < AMOUNT_SETTINGS; i++) {
		items[i] = init_menu_item(setting_points[i]);
	}

	setting_panel_window = newwin(AMOUNT_SETTINGS + 2, 40, 1, 0);
	box(setting_panel_window, 0, 0);
	wattron(setting_panel_window, COLOR_PAIR(MENU_SLCTD_ITEM));
	wbkgd(setting_panel_window, COLOR_PAIR(MENU_SLCTD_ITEM));
	WINDOW* subwin = derwin(setting_panel_window, AMOUNT_SETTINGS, 38, 1, 1);
	SETTINGS_MENU set_menu = NONE_SPRT | NON_DESIG_ITEMS | NON_COL_SIZE | NON_COL_NAME;
	setting_panel_menu = init_menu(items, setting_panel_window, subwin, F_HBOX, set_menu);
	init_color_slctd_item(setting_panel_menu, SLCTD_TOP_PANEL);
	init_menu_format(setting_panel_menu, AMOUNT_SETTINGS, 1);

	print_menu(setting_panel_menu);
}

int show_command_window() {
	show_panel(application[1]);

	int status = -2;
	int key;
	while(status == -2) {
		update_panels();
		doupdate();
		wrefresh(command_panel_menu->subwin);
		key = getch();
		if(key == KEY_DOWN) {
			menu_driver(command_panel_menu, REQ_DOWN_ITEM);
		} else if(key == KEY_UP) {
			menu_driver(command_panel_menu, REQ_UP_ITEM);
		} else if(key == KEY_MOUSE) {
			MEVENT mevent;
			if(getmouse(&mevent) == OK) {
				if(mevent.bstate & BUTTON5_PRESSED) {
					menu_driver(command_panel_menu, REQ_DOWN_ITEM);
				} else if(mevent.bstate & BUTTON4_PRESSED) {
					menu_driver(command_panel_menu, REQ_UP_ITEM);
				} else if(mevent.bstate & BUTTON1_RELEASED) {
					if(!wenclose(command_panel_window, mevent.y, mevent.x)) {
						status = -1;
					} else {
						find_click_item(command_panel_menu, mevent);
					}	
				}
			}
		} else if(key == '\n' || key == KEY_ENTER) {
			status = command_panel_menu->select;
		}
		else if(key == KEY_RESIZE) {
			status = -1;
		}
	}

	hide_panel(application[1]);
	update_panels();
	doupdate();
	return status;
}

int show_setting_panel_window(int panel_num) {
	if(panel_num == 0) {
		mvwin(setting_panel_window, 1, 0);
		mvwin(setting_panel_menu->subwin, 2, 1);
	} else if(panel_num == 1) {
		mvwin(setting_panel_window, 1, 37);
		mvwin(setting_panel_menu->subwin, 2, 38);

	}

	show_panel(application[0]);

	int status = -2;
	int key;
	while(status == -2) {
		update_panels();
		doupdate();
		wrefresh(setting_panel_menu->subwin);
		key = getch();
		if(key == KEY_DOWN) {
			menu_driver(setting_panel_menu, REQ_DOWN_ITEM);
		} else if(key == KEY_UP) {
			menu_driver(setting_panel_menu, REQ_UP_ITEM);
		} else if(key == KEY_MOUSE) {
			MEVENT mevent;
			if(getmouse(&mevent) == OK) {
				if(mevent.bstate & BUTTON5_PRESSED) {
					menu_driver(setting_panel_menu, REQ_DOWN_ITEM);
				} else if(mevent.bstate & BUTTON4_PRESSED) {
					menu_driver(setting_panel_menu, REQ_UP_ITEM);
				} else if(mevent.bstate & BUTTON1_RELEASED) {
					if(!wenclose(setting_panel_window, mevent.y, mevent.x)) {
						status = -1;
					} else {
						find_click_item(setting_panel_menu, mevent);
					}	
				}
			}
		} else if(key == '\n' || key == KEY_ENTER) {
			status = setting_panel_menu->select;
		}
		else if(key == KEY_RESIZE) {
			status = -1;
		}
	}

	hide_panel(application[0]);
	update_panels();
	doupdate();
	return status;
}