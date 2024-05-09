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

#define AMOUNT_COMMAND 4

const wchar_t* command_points[] = {
	L" Символьная ссылка ",
	L" Создать файл ",
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

int command_key_handler(MENU* menu, bool spec_key) {
	int status = -2;
	int key;
	while(status == -2) {
		update_panels();
		doupdate();
		wrefresh(menu->subwin);
		key = getch();
		if(key == KEY_DOWN) {
			menu_driver(menu, REQ_DOWN_ITEM);
		} else if(key == KEY_UP) {
			menu_driver(menu, REQ_UP_ITEM);
		} else if(key == KEY_MOUSE) {
			MEVENT mevent;
			if(getmouse(&mevent) == OK) {
				if(mevent.bstate & BUTTON5_PRESSED) {
					menu_driver(menu, REQ_DOWN_ITEM);
				} else if(mevent.bstate & BUTTON4_PRESSED) {
					menu_driver(menu, REQ_UP_ITEM);
				} else if(mevent.bstate & BUTTON1_RELEASED) {
					if(!wenclose(menu->parwin, mevent.y, mevent.x)) {
						status = -1;
					} else {
						find_click_item(menu, mevent);
					}	
				}
			}
		} else if(key == '\n' || key == KEY_ENTER) {
			status = menu->select;
		}
		else if(key == KEY_RESIZE) {
			status = -1;
		}
		else if(key == KEY_F(1) && spec_key) {
			return KEY_F(1);
		}
		else if(key == KEY_F(2) && spec_key) {
			return KEY_F(2);
		}
	}

	return status;
}

int show_command_window() {
	show_panel(application[1]);
	int status = command_key_handler(command_panel_menu, false); 
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

	int status = command_key_handler(setting_panel_menu, false); 

	hide_panel(application[0]);
	update_panels();
	doupdate();
	return status;
}

int inp_find_pattern(FILE_PANEL* file_panel, wchar_t** rpattern, wchar_t** rsdir) {
	WINDOW* win = cwbcrt_win(9, 82, TOP_PANEL_COLOR, L" ПОИСК ");
	TEXT_BOX* inp_pattern = init_text_box(win, 2, 80, 2, 1, L"", L"Шаблон", NULL, false);
	inp_pattern->box = false;
	show_text_box(inp_pattern);

	TEXT_BOX* inp_sdir = init_text_box(win, 2, 80, 5, 1, L"", L"Стартовый каталог", NULL, false);
	inp_sdir->box = false;
	show_text_box(inp_sdir);
	set_start_text(inp_sdir, file_panel->current_directory);

	MENU* menu = verify_menu(win);
	print_menu(menu);

	int status = -2;
	int key;
	while(status == -2) {
		wrefresh(menu->subwin);
		wrefresh(win);
		key = getch();
		if(key == KEY_RIGHT) {
			menu_driver(menu, REQ_RIGHT_ITEM);
		} else if(key == KEY_LEFT) {
			menu_driver(menu, REQ_LEFT_ITEM);
		} else if(key == KEY_MOUSE) {
			MEVENT mevent;
			if(getmouse(&mevent) == OK) {
				if (mevent.bstate & BUTTON1_RELEASED) {
					if(!wenclose(menu->parwin, mevent.y, mevent.x)) {
						status = -1;
					} else {
						find_click_item(menu, mevent);
					}	
				} 
				else if(wenclose(inp_sdir->form->sub, mevent.y, mevent.x)) {
                    input_text_box(inp_sdir);
                } 
                else if(wenclose(inp_pattern->form->sub, mevent.y, mevent.x)) {
                    input_text_box(inp_pattern);
                }
			}
		} else if(key == '\n' || key == KEY_ENTER) {
			if(menu->select == 1) {
				status = -1;
			} else {
				status = menu->select;
				*rpattern = save_input(inp_pattern);
				*rsdir = save_input(inp_sdir);
			}
		}
		else if(key == KEY_RESIZE) {
			status = -1;
		}
	}

	return status;
}

void find_file(FILE_PANEL* file_panel) {
	WINDOW* window = cwbcrt_win((LINES - 6), (COLS - 6), SLCTD_EXCEPTION_COLOR, L" ПОИСК ");
	mvwaddwstr_color(window, getmaxy(window) - 1, (getmaxx(window) / 4) - 6, L" F1 - переход", EXCEPTION_COLOR);
	mvwaddwstr_color(window, getmaxy(window) - 1, (getmaxx(window) / 2) + (getmaxx(window) / 4) - 5, L" F2 - выход", EXCEPTION_COLOR);
	WINDOW* subwin = wbrt_derwin(window, SLCTD_EXCEPTION_COLOR);

	wchar_t* pattern, *sdir;
	int status = inp_find_pattern(file_panel, &pattern, &sdir);
	if(status == -1) {
		delwin(window);
		delwin(subwin);
		return;
	}

	LIST* flist = init_list(); 
	status = find_files(flist, sdir, pattern);
	if(status == -1) {
		MSG_BOX* msg = init_message_box(5, 30, L" ОШИБКА ", L"Неверное регулярное выражение", false);
		show_msg(msg);
		return;
	}

	MITEM** items = (MITEM**)calloc(flist->size + 1, sizeof(MITEM*));
	for(int i = 0; i < flist->size; i++) {
		if(get_n_element(flist, i)->data->file_type == DIRECTORY) {
			items[i] = init_menu_item(get_n_element(flist, i)->data->full_path);
			items[i]->color = EXCEPTION_COLOR;
			items[i]->pnt_sym = ' ';
		} else {
			items[i] = init_menu_item(get_n_element(flist, i)->data->file_name);
			items[i]->color = SLCTD_EXCEPTION_COLOR;
			items[i]->pnt_sym = ' ';
		}
	}

	SETTINGS_MENU set_menu = NONE_SPRT | DESIG_ITEMS | NON_COL_SIZE | NON_COL_NAME;
	MENU* menu = init_menu(items, window, subwin, GRID, set_menu);
	init_color_slctd_item(menu, TOP_PANEL_COLOR);
	init_menu_format(menu, getmaxy(subwin), 1);
	wnoutrefresh(window);
	wnoutrefresh(subwin);
	doupdate();
	print_menu(menu);

	status = command_key_handler(menu, true);
	if(status == KEY_F(1)) {
		LIST_NODE* finfo = get_n_element(flist, menu->select);
		while(finfo->data->file_type != DIRECTORY) {
			finfo = finfo->prev;
		}
		free(file_panel->current_directory);
		file_panel->current_directory = (wchar_t*)calloc(wcslen(finfo->data->full_path) + 1, sizeof(wchar_t));
		wcscpy(file_panel->current_directory, finfo->data->full_path);
		set_new_items(file_panel->file_menu, load_dir(file_panel), 0);
		while(wcscmp(menu->iselect->string, file_panel->file_menu->iselect->string) != 0) {
			wkeypad_handler(wpanel, KEY_DOWN);
		}
	}

	free_list(flist);
	free_menu(menu);
	delwin(window);
	delwin(subwin);
}