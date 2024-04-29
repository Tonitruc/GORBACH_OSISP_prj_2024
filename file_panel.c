#include "file_panel.h"
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

bool init_file_menu(FILE_PANEL* file_panel, MITEM** items) {
    int row, column;
    getmaxyx(file_panel->panel, row, column);

    file_panel->menu_sub_win = derwin(file_panel->panel, row - FILE_MENU_FORMAT_SIZE_ROW - SPACE_SELECTED_ELEMENT_ROW, 
                                        column - FILE_MENU_FORMAT_SIZE_COL, 
                                        FILE_MENU_START_ROW, FILE_MENU_START_COL);
    wbkgdset(file_panel->menu_sub_win, getbkgd(file_panel->panel));

    SETTINGS_MENU set_menu;
    STNDRT_SETTINGS_F_HBOX(set_menu);
    file_panel->file_menu = init_menu(items, file_panel->panel, file_panel->menu_sub_win, F_HBOX, set_menu);
    init_menu_format(file_panel->file_menu, row - FILE_MENU_FORMAT_ROW - SPACE_SELECTED_ELEMENT_ROW, FILE_MENU_FORMAT_COL);
    set_columns_size(file_panel->file_menu, (double*)3, 1.0, 10.0, 17.0);
    set_column_name(file_panel->file_menu, L"Имя", L"Размер", L"Время правки");
    init_color_slctd_item(file_panel->file_menu, MENU_SLCTD_ITEM);
    file_panel->file_menu->is_sort = true;

    return true;
}

void init_tabs_list(FILE_PANEL* file_panel) {
	MITEM** tabs_items = (MITEM**)calloc(2, sizeof(MITEM*));
	tabs_items[0] = init_menu_item(L"[ + ]");

	WINDOW* sub_window = derwin(file_panel->panel, 1, getmaxx(file_panel->panel) - 4, TAB_LIST_ROW, TAB_LIST_COL);

	init_color(66, 160, 0, 580);
	init_pair(66, COLOR_WHITE, 66);

	SETTINGS_MENU set_menu = SPRT_INTERMEDIATE | NON_DESIG_ITEMS | NON_COL_SIZE | NON_COL_NAME;
	file_panel->tabs_menu = init_menu(tabs_items, file_panel->panel, sub_window, GRID, set_menu);
	file_panel->tabs_menu->sprt_sym = L' ';
	file_panel->tabs_menu->slctd_item_color_pair = 66;
	init_menu_format(file_panel->tabs_menu, 1, 1);

    file_panel->amount_tabs = 0;
	print_menu(file_panel->tabs_menu);
}

FILE_PANEL* init_file_panel(WINDOW* parent_window, int num) {
    FILE_PANEL* file_panel = (FILE_PANEL*)calloc(1, sizeof(FILE_PANEL));

    int height, width, starty, startx;
	FILE_PANEL_SIZE(height, width);
	PANEL_START_POS(starty, startx, num);

    file_panel->parent_window = parent_window;
    file_panel->panel = derwin(parent_window, height, width, starty, startx);
    wbkgdset(file_panel->panel , getbkgd(parent_window));

    keypad(file_panel->panel, TRUE);

    file_panel->current_directory = get_cur_dir();
    MITEM** dir_file = load_dir(file_panel);
    init_file_menu(file_panel, dir_file);

    init_tabs_list(file_panel);

    return file_panel;
}

void refresh_file_panel(FILE_PANEL* file_panel) {
    box(file_panel->panel, 0, 0);
    print_selected_file_space(file_panel);
    print_tab_space(file_panel);
    unprint_menu(file_panel->file_menu);
    print_menu(file_panel->file_menu);
    unprint_menu(file_panel->tabs_menu);
    print_menu(file_panel->tabs_menu);
    wrefresh(file_panel->panel);
}

bool resize_file_panel(FILE_PANEL* file_panel, int num)  {
    int height, width, starty, startx;
    PANEL_START_POS(starty, startx, num);
    FILE_PANEL_SIZE(height, width);

    wresize(file_panel->panel, height, width);    
    mvwin(file_panel->panel, starty, startx);
    mvderwin(file_panel->panel, starty, startx);

    wresize(file_panel->menu_sub_win, height - FILE_MENU_FORMAT_SIZE_ROW - SPACE_SELECTED_ELEMENT_ROW,   
                width - FILE_MENU_FORMAT_SIZE_COL);
    mvderwin(file_panel->menu_sub_win, FILE_MENU_START_ROW, FILE_MENU_START_COL);
    mvwin(file_panel->menu_sub_win, starty + FILE_MENU_START_ROW, startx + FILE_MENU_START_COL);

    wresize(file_panel->tabs_menu->sub_window, 1, getmaxx(file_panel->panel) - 4);    
    mvderwin(file_panel->tabs_menu->sub_window, TAB_LIST_ROW, TAB_LIST_COL);
    mvwin(file_panel->tabs_menu->sub_window, TAB_LIST_ROW, startx + TAB_LIST_COL);

    resize_menu(file_panel->file_menu);
    resize_menu(file_panel->tabs_menu);
    werase(file_panel->panel);

    return true;
}

wchar_t* get_select_file(FILE_PANEL* file_panel) {
    size_t cur_dir_size = wcslen(file_panel->current_directory);
    wchar_t* cur_file;
    if(wcscmp(file_panel->file_menu->items[file_panel->file_menu->selected_item]->string, DIR_RETURN) == 0) {
        for(int i = cur_dir_size - 1; file_panel->current_directory[i] != L'/'; i--) {
            cur_dir_size--;
        }
        cur_file = (wchar_t*)calloc(cur_dir_size, sizeof(wchar_t));
        swprintf(cur_file, cur_dir_size, L"%ls", file_panel->current_directory);
    } else {
        cur_dir_size += file_panel->file_menu->items[file_panel->file_menu->selected_item]->size + 2;
        cur_file = (wchar_t*)calloc(cur_dir_size, sizeof(wchar_t));

        swprintf(cur_file, cur_dir_size, L"%ls/%ls", file_panel->current_directory, 
            file_panel->file_menu->items[file_panel->file_menu->selected_item]->string);
    }

    return cur_file;
}

MITEM** list_to_mitem(List* head) {
    MITEM **items = (MITEM**)calloc(sizeof_list(head) * FILE_MENU_FORMAT_COL + 1, sizeof(MITEM*));

    List* temp = head;
    int num = 0;
    while(temp != NULL) {
        for(int j = 0; j < FILE_MENU_FORMAT_COL; j++, num++) {
            if(j == 0) {
                items[num] = init_menu_item(temp->data->file_name); 
                switch (temp->data->file_type)
                {
                case DIRECTORY: {items[num]->color = MENU_YELLOW; items[num]->pnt_sym = '/'; break; }
                case PDF: {items[num]->color = MENU_RED; items[num]->pnt_sym = ' '; break; }
                case DOC: {items[num]->color = MENU_BLUE; break; }
                case IMAGE: {items[num]->color = MENU_GREEN; break; }
                case ARCHIVE: {items[num]->color = MENU_GRAY; break;}
                default: items[num]->color = MENU_WHITE; items[num]->pnt_sym = '*';
                }
            }
            else if(j == 1) {
                char buffer[20];
                sprintf(buffer, "%ld", temp->data->size_kb);
                items[num] = init_menu_item(cstowchs(buffer));    
            }
            else if(j == 2) {
                items[num] = init_menu_item(wtime(temp->data->edit_time));   
            }
        }
        temp = temp->next;    
    }

    return items;
}

MITEM** load_dir(FILE_PANEL* file_panel) {
    char* cur_dir = wchtochs(file_panel->current_directory);
    List* head = read_dir(cur_dir);
    chdir(cur_dir);
    file_panel->files_info = head;

    MITEM** list_files = list_to_mitem(head);

    free(cur_dir);
    return list_files;
}

bool change_dir(FILE_PANEL* file_panel) {
    wchar_t* new_dir = get_select_file(file_panel);

    free(file_panel->current_directory);
    free_list(file_panel->files_info);
    file_panel->current_directory = new_dir;

    MITEM** new_items = load_dir(file_panel);
    set_new_items(file_panel->file_menu, new_items);
    unprint_menu(file_panel->file_menu);

    return true;
}


bool open_file(FILE_PANEL* file_panel) {
    wchar_t* cur_file = get_select_file(file_panel);
    FILE_TYPE file_type = get_file_type(cur_file);
    const char* args[] = { wchtochs(file_panel->file_menu->items[file_panel->file_menu->selected_item]->string), NULL};


    switch(file_type) {
        case DIRECTORY: {
            if(change_dir(file_panel)) {
                print_current_directory(file_panel, true);
                print_menu(file_panel->file_menu);
            }
            break;
        }
        case IMAGE: 
        case DOC: 
        case PDF: {
            start_app("xdg-open", args);
            break;
        }
        default: break;
    }

    return true;
}

bool mouse_event_handler(FILE_PANEL *file_panel, MEVENT mevent) {
    switch (mevent.bstate)
    {
        case BUTTON5_PRESSED: {
		    menu_driver(file_panel->file_menu, REQ_DOWN_ITEM);
            break;	
		}
		case BUTTON4_PRESSED: {
			menu_driver(file_panel->file_menu, REQ_UP_ITEM);	
            break;
		}
        case BUTTON1_RELEASED: {
            REQ_KEY req_key;
            int prev_tabs = file_panel->tabs_menu->selected_item;
            req_key = find_click_item(file_panel->tabs_menu, mevent);
            if(req_key != NON_REQ && ((req_key == SAME_ITEM && file_panel->amount_tabs == 0)
                || file_panel->tabs_menu->selected_item == file_panel->amount_tabs)) {
                if(!save_tab(file_panel)) {
                    file_panel->tabs_menu->selected_item = prev_tabs;
                }
                return true;
            } else if (req_key == ITEM_CLICKED) {
                load_tab(file_panel, prev_tabs);
                return true;
            } 
            req_key = find_click_item(file_panel->file_menu, mevent);
            if(req_key == COLUMN_CLICKED) {
                file_panel->sort_dir = file_panel->sort_dir == 1? 0 : 1;
                if(file_panel->file_menu->sorted_col == 0) {
                    sort_list(file_panel->files_info->next, get_tail(file_panel->files_info), finfo_name_compare, file_panel->sort_dir);
                } 
                else if(file_panel->file_menu->sorted_col == 1) {
                    sort_list(file_panel->files_info->next, get_tail(file_panel->files_info), finfo_size_compare, file_panel->sort_dir);
                }
                else if(file_panel->file_menu->sorted_col == 2) {
                    sort_list(file_panel->files_info->next, get_tail(file_panel->files_info), finfo_time_compare, file_panel->sort_dir);
                }
                
                set_new_items(file_panel->file_menu, list_to_mitem(file_panel->files_info));
                unprint_menu(file_panel->file_menu);
                print_menu(file_panel->file_menu);
            }
            break;
		}
        default: break;
    }

    print_current_file(file_panel, true);
    wrefresh(file_panel->panel);

    return true;
}

bool keyboard_event_handler(FILE_PANEL *file_panel, int key) {
    switch (key)
    {
        case KEY_DOWN: {
			menu_driver(file_panel->file_menu, REQ_DOWN_ITEM);
            break;	
		}
		case KEY_UP: {
			menu_driver(file_panel->file_menu, REQ_UP_ITEM);	
            break;
		}
        case '\n': {
            open_file(file_panel);
            break;
        }
        case '8': {
			del_file(file_panel);
            break;
		}
		case '6': {
			create_dir(file_panel);
            break;
		}
		case '2': {
			correction_file(file_panel);
            break;
		}
		case '3': {
			open_file(file_panel);
            break;
		}
		case '7': {
			rename_dir(file_panel);
            break;
		}
        case '4': {
			dialog_win_cm(file_panel, L" Перенос ");
            break;
		}
        default: break;
    }

    print_current_file(file_panel, true);
    wrefresh(file_panel->panel);

    return true;
}

void print_selected_file_space(FILE_PANEL* file_panel) {
    int height, width;
    SELECTED_FILE_SPACE_SIZE(file_panel->panel, height, width);

    wchar_t HLINE = U_HLINE;
    for(int i = 1; i < width; i++) {
        mvwaddnwstr(file_panel->panel, height, i, &HLINE, 1);
    }
    mvwaddwstr(file_panel->panel, height, 1, CURRENT_FILE_LOG);

    print_current_file(file_panel, false);
}

void print_current_file(FILE_PANEL *file_panel, bool with_clear) {
    int y, x;
    CURRENT_FILE_POS(file_panel->panel, y, x);

    if(with_clear) {
        clear_y_str(file_panel->panel, y, 1, -1);
    }

    bool is_free = false;
    wchar_t* buffer = file_panel->file_menu->items[file_panel->file_menu->selected_item]->string;
    if(wcslen(buffer) > CUR_FILE_SIZE(file_panel->panel)) {
        buffer = standart_abreviated(buffer, CUR_FILE_SIZE(file_panel->panel));
        is_free = true;
    }
    if(wcscmp(buffer, DIR_RETURN) == 0) {
        mvwaddwstr(file_panel->panel, y, x, DIR_RETURN_NAME);
    } else {
        mvwaddwstr(file_panel->panel, y, x, buffer);
    }
    if(is_free) {
        free(buffer);
    }
}

void print_tab_space(FILE_PANEL *file_panel) {
    int height, width;
    TAB_SPACE_SIZE(file_panel->panel, height, width);

    wchar_t HLINE = U_HLINE;
    for(int i = 1; i < width; i++) {
        mvwaddnwstr(file_panel->panel, height, i, &HLINE, 1);
    }
    mvwaddwstr(file_panel->panel, 0, 1, TAB_NAME);

    print_current_directory(file_panel, false);
}

void print_current_directory(FILE_PANEL *file_panel, bool with_clear) {
    int row, column;
    TAB_SPACE_POS(row, column);

    if(with_clear) {
        clear_y_str(file_panel->panel, row, 1, -1);
    }

    bool is_free = false;
    wchar_t* buffer = file_panel->current_directory;
    if(wcslen(buffer) > CUR_FILE_SIZE(file_panel->panel)) {
        buffer = standart_abreviated(buffer, CUR_FILE_SIZE(file_panel->panel));
        is_free = true;
    }
    mvwaddwstr(file_panel->panel, row, column, buffer);

    if(is_free) {
        free(buffer);
    }
}

int del_file(FILE_PANEL* file_panel) {
    int status = -1;
    wchar_t* buffer = get_select_file(file_panel);

    MSG_BOX* msg = init_message_box(5, 30, L"Удаление", L"Удалить файл?", true);
    set_color_msg(msg, WARNING_BOX_COLOR);
    status = show_msg(msg);

    if(status != -1) {
        status = delete_file(buffer);
        if(status != 0) {
            MSG_BOX* exp = init_message_box(5, 30, L"Ошибка", L"Не удалось удалить файл", false);
            show_msg(exp);
        } else {
            set_new_items(file_panel->file_menu, load_dir(file_panel));
        }
    }

    free(buffer);
    unprint_menu(file_panel->file_menu);
    print_menu(file_panel->file_menu);

    return status;
}

int create_dir(FILE_PANEL* file_panel) {
    int status = -1;
    TEXT_BOX* inp_file_name = init_text_box(stdscr, 7, 40, 0, 0, L"Создание каталога", L"Ведите имя каталога", NULL, true);
    char* name = NULL;
    status = input_text_box(inp_file_name, &name);
    if(status == 0) {
        MSG_BOX* exp = init_message_box(5, 30, L"Ошибка", L"Имя не должно содержать /", false);
        show_msg(exp); 
        return status;
    }
    else if(status == -1) {
        return status;
    } 
    else if(status == 1 && name == NULL) {
        MSG_BOX* exp = init_message_box(5, 30, L"Ошибка", L"Пустое имя", false);
        show_msg(exp); 
    }
    else {
        wchar_t* wname = cstowchs(name);
        size_t size = wcslen(wname) + wcslen(file_panel->current_directory) + 2;
        wchar_t* full_path = (wchar_t*)calloc(size, sizeof(wchar_t));
        swprintf(full_path, size, L"%ls/%ls", file_panel->current_directory, wname);
        status = create_directory(full_path);
        if(status != 1) {
            MSG_BOX* exp = init_message_box(5, 30, L"Ошибка", L"Каталог уже существует", false);
            show_msg(exp);
        } else {
            set_new_items(file_panel->file_menu, load_dir(file_panel));
        }
        free(wname);
        free(full_path);
    }
    
    free(name);
    unprint_menu(file_panel->file_menu);
    print_menu(file_panel->file_menu);

    return status;
}

bool correction_file(FILE_PANEL* file_panel) {
    wchar_t* current_file = get_select_file(file_panel);
    FILE_TYPE file_type = get_file_type(current_file);

    if(file_type == DIRECTORY) {
        MSG_BOX* msg = init_message_box(5, 30, L"Ошибка", L"Это каталог", false);
        show_msg(msg);
        return false;
    }
    const char* args[] = { wchtochs(file_panel->file_menu->items[file_panel->file_menu->selected_item]->string), NULL};
    start_app("nano", args);

    return true;
}

int rename_dir(FILE_PANEL* file_panel) {
    int status = -1;
    TEXT_BOX* inp_file_name = init_text_box(stdscr, 7, 40, 0, 0, L"Новое имя", L"Ведите имя файла", UNIX_FILE_NAME_PATTERN, true);
    char* name;
    status = input_text_box(inp_file_name, &name);
    if(status == 0) {
        MSG_BOX* exp = init_message_box(5, 30, L"Ошибка", L"Имя не должно содержать /", false);
        show_msg(exp); 
        return status;
    }
    else if(status == -1) {
        return status;
    } 
    else if(status == 1 && name == NULL) {
        MSG_BOX* exp = init_message_box(5, 30, L"Ошибка", L"Пустое имя", false);
        show_msg(exp); 
    }
    else {
        wchar_t* wname = cstowchs(name);
        size_t new_size = wcslen(wname) + wcslen(file_panel->current_directory) + 2;
        wchar_t* new_path = (wchar_t*)calloc(new_size, sizeof(wchar_t));
        size_t old_size = file_panel->file_menu->items[file_panel->file_menu->selected_item]->size + wcslen(file_panel->current_directory) + 2;
        wchar_t* old_path = (wchar_t*)calloc(old_size, sizeof(wchar_t));
        swprintf(new_path, new_size, L"%ls/%ls", file_panel->current_directory, wname);
        swprintf(new_path, old_size, L"%ls/%ls", file_panel->current_directory, wname);
        status = move_file(file_panel->file_menu->items[file_panel->file_menu->selected_item]->string, new_path);
        if(status != 1) {
            MSG_BOX* exp = init_message_box(5, 30, L"Ошибка", L"Каталог уже существует", false);
            show_msg(exp);
        } else {
            set_new_items(file_panel->file_menu, load_dir(file_panel));
        }
        free(wname);
        free(new_path);
        free(old_path);
    }
    
    free(name);
    unprint_menu(file_panel->file_menu);
    print_menu(file_panel->file_menu);

    return status;
}

int dialog_win_cm(FILE_PANEL* file_panel, wchar_t* title) {
    int start_x, start_y;
    CENTER_SCR(start_y, start_x);
    WINDOW* window = newwin(15, 40, start_y - 5, start_x - 20);
    keypad(window, true);
    wattron(window, COLOR_PAIR(TOP_PANEL_COLOR));
    wbkgd(window, COLOR_PAIR(TOP_PANEL_COLOR));
    box(window, 0, 0);
    mvwprintw(window, 0, (getmaxx(window) / 2) - (wcslen(title) / 2) - 1, "%ls", title);

    TEXT_BOX* from = init_text_box(window, 2, 38, 2, 1, L"", L"из:", UNIX_FILE_NAME_PATTERN, false);
    from->box = false;
    show_text_box(from);

    TEXT_BOX* to = init_text_box(window, 2, 38, 4, 1, L"", L"в:", UNIX_FILE_NAME_PATTERN, false);
    to->box = false;
    show_text_box(to);

    TOGGLE_BUTTON* sym_link = create_tbutton(7, 3, window, L" Разыменовывание ссылки", UNCHECKED);
    TOGGLE_BUTTON* save_atr = create_tbutton(9, 3, window, L" Сохранить атрибуты", CHECKED);
    TOGGLE_BUTTON* otn_link = create_tbutton(11, 3, window, L" Изменять относительные ссылки", UNCHECKED);

    WINDOW* subwin = derwin(window, 1, 38, 14, 1);
    MITEM **items = (MITEM**)calloc(3, sizeof(MITEM));
    items[0] = init_menu_item(L"[ ОК ]");
    items[1] = init_menu_item(L"[ ОТМЕНА ]");

    SETTINGS_MENU set_menu = NONE_SPRT | NON_DESIG_ITEMS | USER_COL_SIZE | NON_COL_NAME | ALLIGMENT_CENTER;
    MENU* menu = init_menu(items, window, subwin, GRID, set_menu);
    init_menu_format(menu, 1, 2);
    menu->slctd_item_color_pair = SLCTD_EXCEPTION_COLOR; 
    set_columns_size(menu, (double*)2, 0.5, 0.5);

    print_menu(menu);

    char* from_str;
    char* to_str;

    int key; int status = -2;
    while(status == -2) {
        wnoutrefresh(window);
        wnoutrefresh(subwin);
        doupdate();

        key = wgetch(window);
        if(key == '\n' || key == KEY_ENTER) {
            if(menu->selected_item == 0) {
                status = 1;
            }
            else if(menu->selected_item == 1) {
                status = -1;
            }
        } 
        else if(key == KEY_LEFT) {
            menu_driver(menu, REQ_LEFT_ITEM);
        }
        else if(key == KEY_RIGHT) {
            menu_driver(menu, REQ_RIGHT_ITEM);
        }
        else if(key == KEY_MOUSE) {
            MEVENT mevent;
            if(getmouse(&mevent) == OK) {
                if(mevent.bstate & BUTTON1_RELEASED) {
                    if(find_click_item(menu, mevent) != NON_REQ) {
                        continue;
                    }

                    if(wenclose(from->form->sub, mevent.y, mevent.x)) {
                        input_text_box(from, &from_str);
                    } 
                    else if(wenclose(to->form->sub, mevent.y, mevent.x)) {
                        input_text_box(to, &to_str);
                    }

                    wmouse_trafo(window, &mevent.y, &mevent.x, FALSE);
                    if(check_click_tbutton(sym_link, &mevent)) {
                        refersh_button(sym_link);
                    } 
                    else if(check_click_tbutton(save_atr, &mevent)) {
                        refersh_button(save_atr);
                    } 
                    else if(check_click_tbutton(otn_link, &mevent)) {
                        refersh_button(otn_link);
                    } 
                }
            }
        }
    }
    
    return status;
}

void free_file_panel(FILE_PANEL* file_panel) {

}

bool save_tab(FILE_PANEL* file_panel) {
    if(file_panel->amount_tabs == MAX_TABS) {
        MSG_BOX* msg = init_message_box(10, 30, L"Ошибка", L"Максимум 10 влкадок", false);
        show_msg(msg);
        return false;
    }

	TEXT_BOX* tabs_name = init_text_box(stdscr, 6, 40, 0, 0, L" ВВОД ", L"Введите имя вкладки", NULL, true);
	char* name;
	TEXT_REQ status = input_text_box(tabs_name, &name);

	if(status == T_CANCEL) {
		return false;
	} else {
		wchar_t* buffer = cstowchs(name);
		wchar_t* buffer2 = (wchar_t*)calloc(wcslen(buffer) + 6, sizeof(wchar_t));
		swprintf(buffer2, wcslen(buffer) + 5, L"[ %ls ]", buffer);
		add_item(file_panel->tabs_menu, init_menu_item(buffer2), file_panel->amount_tabs++);
		init_menu_format(file_panel->tabs_menu, 1, file_panel->amount_tabs + 1);

		file_panel->tabs[file_panel->amount_tabs - 1].directory = (wchar_t*)calloc(wcslen(file_panel->current_directory) + 1, sizeof(wchar_t));
		file_panel->tabs->selected_item = file_panel->file_menu->selected_item;
		wcscpy(file_panel->tabs[file_panel->amount_tabs - 1].directory , file_panel->current_directory);

		unprint_menu(file_panel->tabs_menu);
		print_menu(file_panel->tabs_menu);

		free(buffer);
	}

	return true;
}

void load_tab(FILE_PANEL* file_panel, int prev_tab_num) {
    file_panel->tabs[prev_tab_num].directory = file_panel->current_directory;
    file_panel->tabs[prev_tab_num].selected_item = file_panel->file_menu->selected_item;

    file_panel->current_directory = (wchar_t*)calloc(wcslen(file_panel->tabs[file_panel->tabs_menu->selected_item].directory) + 1, sizeof(wchar_t));
    wcscpy(file_panel->current_directory, file_panel->tabs[file_panel->tabs_menu->selected_item].directory);

    MITEM** items = load_dir(file_panel);
    set_new_items(file_panel->file_menu, items);
    file_panel->file_menu->selected_item = file_panel->tabs[file_panel->tabs_menu->selected_item].selected_item;
    calc_item_coord(file_panel->file_menu);

    print_menu(file_panel->file_menu);
}