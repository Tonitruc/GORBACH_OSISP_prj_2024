#include "file_panel.h"
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

//Init file panel

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
	MITEM** tabs_panel_items = (MITEM**)calloc(4, sizeof(MITEM*));
    tabs_panel_items[0] = init_menu_item(L"[ ^ ]");
	tabs_panel_items[1] = init_menu_item(L"[ + ]");
	tabs_panel_items[2] = init_menu_item(L"[ - ]"); 

	WINDOW* sub_window = derwin(file_panel->panel, 1, getmaxx(file_panel->panel) - 4, TAB_LIST_ROW, TAB_LIST_COL);

	SETTINGS_MENU set_menu = SPRT_INTERMEDIATE | NON_DESIG_ITEMS | NON_COL_SIZE | NON_COL_NAME;
	file_panel->tabs_panel = init_menu(tabs_panel_items, file_panel->panel, sub_window, GRID, set_menu);
	file_panel->tabs_panel->sprt_sym = L' ';
	file_panel->tabs_panel->slctd_item_color_pair = MENU_TAB_PANEL;
	init_menu_format(file_panel->tabs_panel, 1, 3);

	print_menu(file_panel->tabs_panel);

    file_panel->amount_tabs = 0;

    WINDOW* tab_win = crt_box_win(12, 40, 5, getcurx(file_panel->panel) + 2, SLCTD_EXCEPTION_COLOR, L"");
    WINDOW* subwin = wbrt_derwin(tab_win, SLCTD_EXCEPTION_COLOR);

    MITEM** tabs_items = (MITEM**)calloc(1, sizeof(MITEM*));
    set_menu = NONE_SPRT | DESIG_ITEMS | NON_COL_SIZE | NON_COL_NAME;
	file_panel->tabs_menu = init_menu(tabs_items, tab_win, subwin, GRID, set_menu);
	file_panel->tabs_menu->slctd_item_color_pair = TOP_PANEL_COLOR;
	init_menu_format(file_panel->tabs_menu, 10, 1);

    print_menu(file_panel->tabs_menu);
}

FILE_PANEL* init_file_panel(WINDOW* parent_window, int num) {
    FILE_PANEL* file_panel = (FILE_PANEL*)calloc(1, sizeof(FILE_PANEL));
    file_panel->files_info = init_list();

    int height, width, starty, startx;
	FILE_PANEL_SIZE(height, width);
	PANEL_START_POS(starty, startx, num);

    file_panel->parent_window = parent_window;
    file_panel->panel = derwin(parent_window, height, width, starty, startx);
    wbkgdset(file_panel->panel , getbkgd(parent_window));

    keypad(file_panel->panel, TRUE);

    file_panel->sort_dir = 0;
    file_panel->is_sort = false;
    file_panel->current_directory = get_cur_dir();
    MITEM** dir_file = load_dir(file_panel);
    init_file_menu(file_panel, dir_file);

    init_tabs_list(file_panel);
    return file_panel;
}

//update file panel

void refresh_file_panel(FILE_PANEL* file_panel) {
    box(file_panel->panel, 0, 0);
    print_selected_file_space(file_panel);
    print_tab_space(file_panel);
    print_current_file(file_panel, true);

    unprint_menu(file_panel->file_menu);
    print_menu(file_panel->file_menu);
    unprint_menu(file_panel->tabs_panel);
    print_menu(file_panel->tabs_panel);
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

    wresize(file_panel->tabs_panel->subwin, 1, getmaxx(file_panel->panel) - 4);    
    mvderwin(file_panel->tabs_panel->subwin, TAB_LIST_ROW, TAB_LIST_COL);
    mvwin(file_panel->tabs_panel->subwin, starty + TAB_LIST_ROW, startx + TAB_LIST_COL);

    resize_menu(file_panel->file_menu);
    resize_menu(file_panel->tabs_panel);
    werase(file_panel->panel);

    mvwin(file_panel->tabs_menu->parwin, 5, getcurx(file_panel->panel) + 2);
    mvwin(file_panel->tabs_menu->subwin, 6, getcurx(file_panel->panel) + 3);

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

void print_current_tab(FILE_PANEL* file_panel) {
    unprint_menu(file_panel->tabs_panel);
    free(file_panel->tabs_panel->items[0]);

    if(file_panel->amount_tabs == 0) {
        file_panel->tabs_panel->items[0] = init_menu_item(L"[ ^ ]");
    } else {
        file_panel->tabs_panel->items[0] = init_menu_item(file_panel->tabs_menu->iselect->string);
    }

    calc_item_coord(file_panel->tabs_panel);
    print_menu(file_panel->tabs_panel);
}

void print_current_file(FILE_PANEL *file_panel, bool with_clear) {
    int y, x;
    CURRENT_FILE_POS(file_panel->panel, y, x);

    if(with_clear) {
        clear_y_str(file_panel->panel, y, 1, -1);
    }

    bool is_free = false;
    wchar_t* buffer;
    if(get_n_element(file_panel->files_info ,mslct(file_panel->file_menu))->data->file_type == SYMBOL_LINK ||
        get_n_element(file_panel->files_info ,mslct(file_panel->file_menu))->data->file_type == BAD_SYMBOL_LINK) {
        wchar_t* link = get_select_file(file_panel);
        wchar_t* path = find_slnk_path(link);
        size_t size = wcslen(file_panel->file_menu->iselect->string) + wcslen(path) + 5;
        buffer = (wchar_t*)calloc(size, sizeof(wchar_t));
        swprintf(buffer, size, L"%ls -> %ls", file_panel->file_menu->iselect->string, path);
        is_free = true;
        free(link);
        free(path);
    } 
    else {
        buffer = file_panel->file_menu->iselect->string;
    } 
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

//help function

wchar_t* get_select_file(FILE_PANEL* file_panel) {
    size_t cur_dir_size = wcslen(file_panel->current_directory);
    wchar_t* cur_file;
    if(wcscmp(file_panel->file_menu->iselect->string, DIR_RETURN) == 0) {
        if(wcscmp(file_panel->current_directory, L"/home") == 0) {
            cur_file = (wchar_t*)calloc(2, sizeof(wchar_t));
            cur_file[0] = '/'; cur_file[1] = '\0';
        } else {
            for(int i = cur_dir_size - 1; file_panel->current_directory[i] != L'/'; i--) {
                cur_dir_size--;
            }
            cur_file = (wchar_t*)calloc(cur_dir_size, sizeof(wchar_t));
            swprintf(cur_file, cur_dir_size, L"%ls", file_panel->current_directory);
        }
    } else {
        if(wcscmp(file_panel->file_menu->iselect->string, L"home") == 0 
            && wcscmp(file_panel->current_directory, L"/") == 0) {
            cur_file = (wchar_t*)calloc(6, sizeof(wchar_t));
            swprintf(cur_file, 6, L"/home");
        } else {
            cur_dir_size += file_panel->file_menu->iselect->size + 2;
            cur_file = (wchar_t*)calloc(cur_dir_size, sizeof(wchar_t));

            swprintf(cur_file, cur_dir_size, L"%ls/%ls", file_panel->current_directory, 
                file_panel->file_menu->iselect->string);
        }
    }

    return cur_file;
}

MITEM** init_files(LIST* list) {
    MITEM **items = (MITEM**)calloc(list->size * FILE_MENU_FORMAT_COL + 1, sizeof(MITEM*));

    LIST_NODE* temp = list->head;
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
                case SYMBOL_LINK: {items[num]->color = MENU_BLUE; items[num]->pnt_sym = '@'; break;}
                case BAD_SYMBOL_LINK: { items[num]->color = MENU_RED; items[num]->pnt_sym = '!'; break;}
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

//view directory fucnitons

MITEM** load_dir(FILE_PANEL* file_panel) {
    char* cur_dir = wchtochs(file_panel->current_directory);
    LIST* list = read_dir(cur_dir);

    if(file_panel->is_sort) {
        if(file_panel->file_menu->sorted_col == 0) {
            sort_list(list->head->next, list->tail, finfo_name_compare, file_panel->sort_dir);
        } 
        else if(file_panel->file_menu->sorted_col == 1) {
            sort_list(list->head->next, list->tail, finfo_size_compare, file_panel->sort_dir);
        }
        else if(file_panel->file_menu->sorted_col == 2) {
            sort_list(list->head->next, list->tail, finfo_time_compare, file_panel->sort_dir);
        }
    } 

    chdir(cur_dir);
    file_panel->files_info = list;

    MITEM** list_files = init_files(list);

    free(cur_dir);
    return list_files;
}

bool change_dir(FILE_PANEL* file_panel) {
    wchar_t* new_dir = get_select_file(file_panel);

    free(file_panel->current_directory);
    free_list(file_panel->files_info);
    file_panel->current_directory = new_dir;

    MITEM** new_items = load_dir(file_panel);
    set_new_items(file_panel->file_menu, new_items, 0);
    unprint_menu(file_panel->file_menu);

    return true;
}

bool open_file(FILE_PANEL* file_panel) {
    wchar_t* cur_file = get_select_file(file_panel);
    FILE_TYPE file_type = get_file_type(cur_file);
    const char* args[] = { wchtochs(file_panel->file_menu->iselect->string), NULL};

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

//functions for work with tabs

int chooce_tab(FILE_PANEL* file_panel) {
    overlay(file_panel->tabs_menu->parwin, file_panel->panel);
    touchwin(file_panel->tabs_menu->parwin);
    wrefresh(file_panel->tabs_menu->parwin);
    print_menu(file_panel->tabs_menu);

    int choice = -1;
    while(choice == -1) {
        wrefresh(file_panel->tabs_menu->subwin);
        int key = wgetch(file_panel->tabs_menu->parwin);
        if(key == KEY_DOWN) {
            menu_driver(file_panel->tabs_menu, REQ_DOWN_ITEM);
        } 
        else if(key == KEY_UP) {
            menu_driver(file_panel->tabs_menu, REQ_UP_ITEM);
        }
        else if(key == KEY_RESIZE) {
            return choice;
        }
        else if(key == '\n' || key == KEY_ENTER) {
           choice = file_panel->tabs_menu->select;
        } 
        else if(key == KEY_MOUSE) {
            MEVENT mevent;
            if(getmouse(&mevent) == OK) {
                if(mevent.bstate & BUTTON1_RELEASED) {
                    find_click_item(file_panel->tabs_menu, mevent);
                }
                else if(mevent.bstate & BUTTON5_PRESSED) {
                    menu_driver(file_panel->tabs_menu, REQ_DOWN_ITEM);
                } 
                else if(mevent.bstate & BUTTON4_PRESSED) {
                    menu_driver(file_panel->tabs_menu, REQ_UP_ITEM);
                } 
                else if(!wenclose(file_panel->tabs_menu->parwin, mevent.y, mevent.x)) {
                    return choice;
                }
            }
        }
    }

    return choice;
}

bool save_tab(FILE_PANEL* file_panel) {
    if(file_panel->amount_tabs == MAX_TABS) {
        MSG_BOX* msg = init_message_box(10, 30, L"Ошибка", L"Максимум 10 влкадок", false);
        show_msg(msg);
        return false;
    }

	TEXT_BOX* tabs_name = init_text_box(stdscr, 6, 40, 0, 0, L" ВВОД ", L"Введите имя вкладки", NULL, true);
	wchar_t* name;
	TEXT_REQ status = input_text_box(tabs_name);
    name = save_input(tabs_name);

	if(status == T_CANCEL) {
		return false;
	} else {
		wchar_t* buffer = (wchar_t*)calloc(wcslen(name) + 6, sizeof(wchar_t));
		swprintf(buffer, wcslen(name) + 5, L"[ %ls ]", name);
		add_item(file_panel->tabs_menu, init_menu_item(buffer), file_panel->amount_tabs++);

		file_panel->tabs[file_panel->amount_tabs - 1].directory = (wchar_t*)calloc(wcslen(file_panel->current_directory) + 1, sizeof(wchar_t));
		file_panel->tabs->select = file_panel->file_menu->select;
		wcscpy(file_panel->tabs[file_panel->amount_tabs - 1].directory , file_panel->current_directory);

		unprint_menu(file_panel->tabs_menu);
		print_menu(file_panel->tabs_menu);

		free(buffer);
	}

	return true;
}

bool delete_tab(FILE_PANEL* file_panel) {
    unprint_menu(file_panel->tabs_menu);
    if(file_panel->amount_tabs == 0) {
        MSG_BOX* msg = init_message_box(5, 30, L"Ошибка", L"Ни одной вкладки не создано", false);
        show_msg(msg);
        return false;
    } 

    delete_item(file_panel->tabs_menu, file_panel->tabs_menu->select);
    free(file_panel->tabs[file_panel->tabs_menu->select].directory);
    for(int i = file_panel->tabs_menu->select; i < MAX_TABS - 1; i++) {
        file_panel->tabs[i] = file_panel->tabs[i + 1];
    }

    calc_item_coord(file_panel->tabs_menu);
	print_menu(file_panel->tabs_menu);
    file_panel->amount_tabs--;

	return true;
}

void load_tab(FILE_PANEL* file_panel, int prev_tab_num) {
    if(prev_tab_num != -1) {
        file_panel->tabs[prev_tab_num].directory = file_panel->current_directory;
        file_panel->tabs[prev_tab_num].select = file_panel->file_menu->select;
    } 
    int select = 0;
    if(file_panel->amount_tabs == 0) {
        file_panel->current_directory = get_cur_dir();
    } else {
        file_panel->current_directory = (wchar_t*)calloc(wcslen(file_panel->tabs[file_panel->tabs_menu->select].directory) + 1, sizeof(wchar_t));
        wcscpy(file_panel->current_directory, file_panel->tabs[file_panel->tabs_menu->select].directory);
        select = file_panel->tabs[file_panel->tabs_menu->select].select;
    }

    MITEM** items = load_dir(file_panel);
    set_new_items(file_panel->file_menu, items, select);

    calc_item_coord(file_panel->file_menu);
    print_menu(file_panel->file_menu);
}

//key and moust handlers

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
            int prev_tab = file_panel->tabs_menu->select;
            req_key = find_click_item(file_panel->tabs_panel, mevent);
            if(req_key != NON_REQ && file_panel->tabs_panel->select == 0) {
                if(chooce_tab(file_panel) != -1) {
                    load_tab(file_panel, prev_tab);
                    print_current_tab(file_panel);
                }
                return true;
            }
            else if(req_key != NON_REQ && file_panel->tabs_panel->select == 1) {
                if(save_tab(file_panel)) {
                    file_panel->tabs_menu->select = file_panel->amount_tabs - 1;
                    file_panel->tabs_panel->select = 0;
                    print_current_tab(file_panel);
                }
                return true;
            }
            else if (file_panel->tabs_panel->select == 2) {
                if(delete_tab(file_panel)) {
                    file_panel->tabs_menu->select = 0;
                    load_tab(file_panel, -1);
                    print_current_tab(file_panel);
                    file_panel->tabs_panel->select = 0;
                }
                return true;
            } 

            req_key = find_click_item(file_panel->file_menu, mevent);
            if(req_key == COLUMN_CLICKED) {
                file_panel->is_sort = true;
                file_panel->sort_dir = file_panel->sort_dir == 1? 0 : 1;
                if(file_panel->file_menu->sorted_col == 0) {
                    sort_list(file_panel->files_info->head->next, file_panel->files_info->tail, finfo_name_compare, file_panel->sort_dir);
                } 
                else if(file_panel->file_menu->sorted_col == 1) {
                    sort_list(file_panel->files_info->head->next, file_panel->files_info->tail, finfo_size_compare, file_panel->sort_dir);
                }
                else if(file_panel->file_menu->sorted_col == 2) {
                    sort_list(file_panel->files_info->head->next, file_panel->files_info->tail, finfo_time_compare, file_panel->sort_dir);
                }
                
                set_new_items(file_panel->file_menu, init_files(file_panel->files_info), 0);
                unprint_menu(file_panel->file_menu);
                print_menu(file_panel->file_menu);
            }
            break;
		}
        default: break;
    }

    return true;
}

bool keyboard_event_handler(FILE_PANEL *file_panel, int key, FILE_PANEL* dep) {
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
        case KEY_SF: {
            if(file_panel->file_menu->select != 0) {
                menu_driver(file_panel->file_menu, REQ_DOWN_ITEM | GROUP_ACT);
            } else {
			    menu_driver(file_panel->file_menu, REQ_DOWN_ITEM);
            }
            break;
        }
        case KEY_SR: {
            if(file_panel->file_menu->select != 0) {
                menu_driver(file_panel->file_menu, REQ_UP_ITEM | GROUP_ACT);
            }
            break;
        }
        case '\n': {
            open_file(file_panel);
            break;
        }
        case '5': {
            if(file_panel->file_menu->select != 0)
			    copy_files(file_panel, dep);
            break;
		}
		case '6': {
            if(file_panel->file_menu->select != 0)
			    create_dir(file_panel);
            break;
		}
        case '7': {
            if(file_panel->file_menu->select != 0)
			    rename_dir(file_panel);
            break;
		}
        case '8': {
            if(file_panel->file_menu->select != 0)
			    del_file(file_panel);
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
        case 'S': {
            if(file_panel->file_menu->select != 0)
                create_sym_link(file_panel);
            break;
        }
        default: break;
    }

    print_current_file(file_panel, true);
    wrefresh(file_panel->panel);

    return true;
}

//file CRUD

FOPR file_operation_handler(FOPR status) {
    if(status == OPERATION_ERROR) {
        MSG_BOX* exp = init_message_box(7, 40, L" ОШИБКА ", L"Ошибка при выполнении операции.\n  Операция была отменена \n  или выполнена частично", false);
        show_msg(exp); 
    } else if(status == SDIR_NOT_EXIST) {
        MSG_BOX* exp = init_message_box(5, 30, L" ОШИБКА ", L"Исходный каталог не существует.", false);
        show_msg(exp); 
    } else if(status == RDIR_NOT_EXITS) {
        MSG_BOX* exp = init_message_box(5, 30, L" ОШИБКА ", L"Результирующий каталог не существует.", false);
        show_msg(exp); 
    } else if(status == NAME_EXIST) {
        MSG_BOX* exp = init_message_box(5, 30, L" ОШИБКА ", L"Имя уже занято.", false);
        show_msg(exp); 
    } else if(status == EMPTY_NAME) {
        MSG_BOX* exp = init_message_box(5, 30, L" ОШИБКА ", L"Пустое имя.", false);
        show_msg(exp); 
    }
 
    return status;
}

int del_file(FILE_PANEL* file_panel) {
    int status = -1;
    wchar_t* buffer = get_select_file(file_panel);

    MSG_BOX* msg = init_message_box(5, 30, L"Удаление", L"Удалить файл?", true);
    set_color_msg(msg, WARNING_BOX_COLOR);
    status = show_msg(msg);

    if(status != -1) {
        bool is_dir = get_file_type(buffer) == DIRECTORY;
        status = delete_file(buffer, is_dir);
        if(file_operation_handler(status)) {
            set_new_items(file_panel->file_menu, load_dir(file_panel), 0);
        }
    }

    free(buffer);
    unprint_menu(file_panel->file_menu);
    print_menu(file_panel->file_menu);

    return status;
} 

int create_dir(FILE_PANEL* file_panel) {
    int status = -1;
    TEXT_BOX* inp_file_name = init_text_box(stdscr, 7, 40, 0, 0, L" СОЗДАНИЕ ", L"Ведите имя каталога", UNIX_FILE_NAME_PATTERN, true);
    wchar_t* name = NULL;
    status = input_text_box(inp_file_name);
    if(status == -1) {
        return status;
    } 
    else {
        name = save_input(inp_file_name);
        if(name == NULL) {
            MSG_BOX* exp = init_message_box(5, 30, L" ОШИБКА ", L"Имя не должно содержать /", false);
            show_msg(exp); 
            return -1;
        }
        if(wcslen(name) == 0) {
            file_operation_handler(EMPTY_NAME); 
        } else {
            size_t size = wcslen(name) + wcslen(file_panel->current_directory) + 2;
            wchar_t* full_path = (wchar_t*)calloc(size, sizeof(wchar_t));
            swprintf(full_path, size, L"%ls/%ls", file_panel->current_directory, name);
            status = create_directory(full_path);
            if(file_operation_handler(status)) {
            set_new_items(file_panel->file_menu, load_dir(file_panel), 0);
        }
        free(full_path);    
        }
        free(name);
    }

    unprint_menu(file_panel->file_menu);
    print_menu(file_panel->file_menu);

    return status;
}

bool correction_file(FILE_PANEL* file_panel) {
    wchar_t* current_file = get_select_file(file_panel);
    FILE_TYPE file_type = get_file_type(current_file);

    if(file_type == DIRECTORY) {
        MSG_BOX* msg = init_message_box(5, 30, L" ОШИБКА ", L"Это каталог", false);
        show_msg(msg);
        return false;
    }
    const char* args[] = { wchtochs(file_panel->file_menu->iselect->string), NULL};
    start_app("nano", args);

    return true;
}

int rename_dir(FILE_PANEL* file_panel) {
    int status = -1;
    TEXT_BOX* inp_file_name = init_text_box(stdscr, 7, 40, 0, 0, L" ПЕРЕИМЕНОВАНИЕ ", L"Ведите имя файла", UNIX_FILE_NAME_PATTERN, true);
    status = input_text_box(inp_file_name);
    if(status == -1) {
        return status;
    } 
    else {
        wchar_t* new_name = save_input(inp_file_name);
        if(new_name == NULL) {
            MSG_BOX* exp = init_message_box(5, 30, L" ОШИБКА ", L"Имя не должно содержать /", false);
            show_msg(exp); 
            return -1;            
        }
        if(wcslen(new_name) == 0) {
            file_operation_handler(EMPTY_NAME);         
        } else {
            wchar_t* curr = get_select_file(file_panel);
            status = rnm_file(curr, new_name);
            if(file_operation_handler(status)) {
                set_new_items(file_panel->file_menu, load_dir(file_panel), 0);
            }
            free(curr);
        }
        free(new_name);
    }
    
    unprint_menu(file_panel->file_menu);
    print_menu(file_panel->file_menu);

    return status;
} 

int dialog_win_cm(FILE_PANEL* file_panel, wchar_t* title) {
    WINDOW* window = cwbcrt_win(15, 40, TOP_PANEL_COLOR, title);

    TEXT_BOX* from = init_text_box(window, 2, 38, 2, 1, L"", L"из:", UNIX_FILE_NAME_PATTERN, false);
    from->box = false;
    show_text_box(from);

    TEXT_BOX* to = init_text_box(window, 2, 38, 4, 1, L"", L"в:", UNIX_FILE_NAME_PATTERN, false);
    to->box = false;
    show_text_box(to);

    TOGGLE_BUTTON* sym_link = create_tbutton(7, 3, window, L" Разыменовывание ссылки", UNCHECKED);
    TOGGLE_BUTTON* save_atr = create_tbutton(9, 3, window, L" Сохранить атрибуты", CHECKED);

    MENU* menu = verify_menu(window);

    print_menu(menu);

    wchar_t* from_str, * to_str;

    int key; int status = -2;
    while(status == -2) {
        wnoutrefresh(window);
        wnoutrefresh(menu->subwin);
        doupdate();

        key = wgetch(window);
        if(key == '\n' || key == KEY_ENTER) {
            if(menu->select == 0) {
                from_str = save_input(from);
                to_str = save_input(to);
                status = 1;
            }
            else if(menu->select == 1) {
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
                        input_text_box(from);
                    } 
                    else if(wenclose(to->form->sub, mevent.y, mevent.x)) {
                        input_text_box(to);
                    }

                    wmouse_trafo(window, &mevent.y, &mevent.x, FALSE);
                    if(check_click_tbutton(sym_link, &mevent)) {
                        refersh_button(sym_link);
                    } 
                    else if(check_click_tbutton(save_atr, &mevent)) {
                        refersh_button(save_atr);
                    } 
                }
            }
        }
    }
    
    return status;
}

bool create_sym_link(FILE_PANEL* file_panel) {
    WINDOW* window = cwbcrt_win(9, 80, TOP_PANEL_COLOR, L" СОЗДАНИЕ ");

    MENU* menu = verify_menu(window);
    print_menu(menu);

    TEXT_BOX* from = init_text_box(window, 2, 78, 2, 1, L"", L"Имя файла:", NULL, false);
    from->box = false;
    wchar_t* buffer = get_select_file(file_panel);
    show_text_box(from);
    set_start_text(from, buffer);

    TEXT_BOX* to = init_text_box(window, 2, 78, 4, 1, L"", L"Имя ссылки:", UNIX_FILE_NAME_PATTERN, false);
    to->box = false;
    show_text_box(to);

    wchar_t *from_str, *to_str;

    int key; int status = -2;
    while(status == -2) {
        wnoutrefresh(window);
        wnoutrefresh(menu->subwin);
        doupdate();

        key = wgetch(window);
        if(key == '\n' || key == KEY_ENTER) {
            if(menu->select == 0) {
                from_str = save_input(from);
                to_str = save_input(to);
                create_slnk(from_str, to_str);
                set_new_items(file_panel->file_menu, load_dir(file_panel), 0);
                status = 1;
            }
            else if(menu->select == 1) {
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
                        input_text_box(from);
                    } 
                    else if(wenclose(to->form->sub, mevent.y, mevent.x)) {
                        input_text_box(to);
                    }
                }
            }
        }
    }
    free(from_str);
    free(to_str);
    
    return status;
}

bool copy_files(FILE_PANEL* file_panel, FILE_PANEL* dep) {
    wchar_t* copy_file = get_select_file(file_panel);
    size_t size = wcslen(dep->current_directory) + wcslen(file_panel->file_menu->iselect->string) + 2;
    wchar_t* new_dir = (wchar_t*)calloc(size, sizeof(wchar_t));
    swprintf(new_dir, size, L"%ls/%ls", dep->current_directory, file_panel->file_menu->iselect->string);

    int status = dialog_win_cm(file_panel, L" КОПИРОВАНИЕ ");
    if(status != -1) {
        if(get_file_type(copy_file) == DIRECTORY) {
            status = cpydir(copy_file, new_dir);
        } else {
            if(!mcontains(dep->file_menu, file_panel->file_menu->iselect->string)) {
                status = cpyfile(copy_file, new_dir);
            } else {
                status = NAME_EXIST;
            }
        }
        if(file_operation_handler(status)) {
            set_new_items(dep->file_menu, load_dir(dep), 0);
        } else return false;
    } 

    return true;
}

void free_file_panel(FILE_PANEL* file_panel) {
    
} 