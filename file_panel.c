#include "file_panel.h"
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

bool init_file_panel(File_Panel** file_panel, WINDOW* parent_window, int num) {
    *file_panel = (File_Panel*)calloc(1, sizeof(File_Panel));

    int height, width, starty, startx;
	FILE_PANEL_SIZE(height, width);
	PANEL_START_POS(starty, startx, num);

    (*file_panel)->parent_window = parent_window;
    (*file_panel)->panel = derwin(parent_window, height, width, starty, startx);
    if ((*file_panel)->panel == NULL) {    
        return false;
    }
    wbkgdset((*file_panel)->panel , getbkgd(parent_window));
    
    (*file_panel)->current_directory = (wchar_t*)calloc(wcslen(START_DIR) + 1, sizeof(wchar_t));
    wcscpy((*file_panel)->current_directory, START_DIR);

    keypad((*file_panel)->panel, TRUE);

    MITEM** dir_file = load_dir(*file_panel);
    init_file_menu(*file_panel, dir_file);

    return true;
}

bool resize_file_panel(File_Panel* file_panel, int num)  {
    int height, width, starty, startx;
    PANEL_START_POS(starty, startx, num);
    FILE_PANEL_SIZE(height, width);

    wresize(file_panel->panel, height, width);    //resize main window
    mvwin(file_panel->panel, starty, startx);
    mvderwin(file_panel->panel, starty, startx);

    wresize(file_panel->menu_sub_win, height - FILE_MENU_FORMAT_SIZE_ROW - SPACE_SELECTED_ELEMENT_ROW,   //resize subwindow
                width - FILE_MENU_FORMAT_SIZE_COL);
    mvderwin(file_panel->menu_sub_win, FILE_MENU_START_ROW, FILE_MENU_START_COL);
    mvwin(file_panel->menu_sub_win, starty + FILE_MENU_START_ROW, startx + FILE_MENU_START_COL);

    resize_menu(file_panel->file_menu);
    werase(file_panel->panel);

    return true;
}

void refresh_file_panel(File_Panel* file_panel) {
    box(file_panel->panel, 0, 0);
    print_selected_file_space(file_panel);
    print_tab_space(file_panel);
    print_menu(file_panel->file_menu);
    wrefresh(file_panel->panel);
}

MITEM** load_dir(File_Panel* file_panel) {
    char* cur_dir = wchtochs(file_panel->current_directory);
    List* head = read_dir(cur_dir);
    int i = 0;

    MITEM **my_items = (MITEM**)calloc(sizeof_list(head) * FILE_MENU_FORMAT_COL + 1, sizeof(MITEM*));

    List* temp = head;
    file_panel->files_info = head;
    while(temp != NULL) {
        for(int j = 0; j < FILE_MENU_FORMAT_COL; j++) {
            if(j == 0) {
                my_items[i] = init_menu_item(temp->data->file_name); 
                switch (temp->data->file_type)
                {
                    case DIRECTORY: {my_items[i]->color = MENU_YELLOW; my_items[i]->pnt_sym = '/'; break; }
                    case PDF: {my_items[i]->color = MENU_RED; my_items[i]->pnt_sym = ' '; break; }
                    case DOC: {my_items[i]->color = MENU_BLUE; break; }
                    case IMAGE: {my_items[i]->color = MENU_GREEN; break; }
                    case ARCHIVE: {my_items[i]->color = MENU_GRAY; break;}
                    default: my_items[i]->color = MENU_WHITE; my_items[i]->pnt_sym = '*';
                }
            }
            else if(j == 1) {
                char buffer[20];
                sprintf(buffer, "%ld", temp->data->size_kb);
                my_items[i] = init_menu_item(cstowchs(buffer));    
            }
            else if(j == 2) {
                my_items[i] = init_menu_item(wtime(temp->data->edit_time));   
            }
            i++;
        }
        temp = temp->next;    
    }
    my_items[i] = NULL;

    free(cur_dir);

    return my_items;
}

bool change_dir(File_Panel* file_panel) {
    size_t new_dir_size = wcslen(file_panel->current_directory);
    wchar_t* new_dir;
    if(wcscmp(file_panel->file_menu->items[file_panel->file_menu->selected_item]->string, DIR_RETURN) == 0) {
        for(int i = new_dir_size - 1; file_panel->current_directory[i] != L'/'; i--) {
            new_dir_size--;
        }

        new_dir = (wchar_t*)calloc(new_dir_size, sizeof(wchar_t));
        swprintf(new_dir, new_dir_size, L"%ls", file_panel->current_directory);
    } else {
        new_dir_size += wcslen(file_panel->file_menu->items[file_panel->file_menu->selected_item]->string) + 2;
        new_dir = (wchar_t*)calloc(new_dir_size, sizeof(wchar_t));
        swprintf(new_dir, new_dir_size, L"%ls/%ls", file_panel->current_directory, 
            file_panel->file_menu->items[file_panel->file_menu->selected_item]->string);
    }

    FILE_TYPE cur_file_type = get_file_type(new_dir);

    if(cur_file_type != DIRECTORY) {
        free(new_dir);
        return false;
    } 

    free(file_panel->current_directory);
    file_panel->current_directory = new_dir;

    MITEM** new_items = load_dir(file_panel);
    set_new_items(file_panel->file_menu, new_items);
    unprint_menu(file_panel->file_menu);
    return true;
}

bool init_file_menu(File_Panel* file_panel, MITEM** items) {
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
    set_columns_size(file_panel->file_menu, (double*)3, 1.0, 10.0, 14.0);
    set_column_name(file_panel->file_menu, L"Имя", L"Размер", L"Время правки");
    init_color_slctd_item(file_panel->file_menu, MENU_SLCTD_ITEM);
    file_panel->file_menu->is_sort = true;

    return true;
}

bool event_handler(File_Panel *file_panel, int key) {

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
            if(change_dir(file_panel)) {
                print_current_directory(file_panel, true);
                print_menu(file_panel->file_menu);
            }
            break;
        }
        case KEY_MOUSE: {
            MEVENT mevent;
            if (getmouse(&mevent) != OK) { 
                return false;
            }

            if (mevent.bstate & BUTTON5_PRESSED) { 
                menu_driver(file_panel->file_menu, REQ_DOWN_ITEM);
            }
			else if (mevent.bstate & BUTTON4_PRESSED) { 
                menu_driver(file_panel->file_menu, REQ_UP_ITEM);
            }
			else if(mevent.bstate & BUTTON1_RELEASED) {
				REQ_KEY req_key = find_click_item(file_panel->file_menu, mevent);
                if(req_key == COLUMN_CLICKED) {
                    if(file_panel->file_menu->sorted_col == 0) {
                        menu_sort(file_panel->file_menu, file_panel->file_menu->sorted_col, wcompare);
                    } 
                    else if(file_panel->file_menu->sorted_col == 1) {
                        menu_sort(file_panel->file_menu, file_panel->file_menu->sorted_col, wicompare);
                    }
                    else if(file_panel->file_menu->sorted_col == 2) {
                        menu_sort(file_panel->file_menu, file_panel->file_menu->sorted_col, wcompare);
                    }
                    
                    unprint_menu(file_panel->file_menu);
                    print_menu(file_panel->file_menu);
                } 
			}
            break;
        }
        default:break;
    }

    print_current_file(file_panel, true);
    wrefresh(file_panel->panel);

    return true;
}

void print_selected_file_space(File_Panel* file_panel) {
    int height, width;
    SELECTED_FILE_SPACE_SIZE(file_panel->panel, height, width);

    wchar_t HLINE = U_HLINE;
    for(int i = 1; i < width; i++) {
        mvwaddnwstr(file_panel->panel, height, i, &HLINE, 1);
    }

    mvwaddwstr(file_panel->panel, height, 1, CURRENT_FILE_LOG);

    print_current_file(file_panel, false);
}

void print_current_file(File_Panel *file_panel, bool with_clear) {
    int y, x;
    CURRENT_FILE_POS(file_panel->panel, y, x);

    if(with_clear) {
        clear_y_str(file_panel->panel, y, 1, -1);
    }

    bool is_free = false;
    wchar_t* buffer = file_panel->file_menu->items[file_panel->file_menu->selected_item]->string;
    if(wcslen(buffer) > getmaxx(file_panel->panel) - 4) {
        buffer = standart_abreviated(buffer, getmaxx(file_panel->panel) - 4);
    }
    if(wcscmp(buffer, DIR_RETURN) == 0) {
        mvwaddwstr(file_panel->panel, y, x, DIR_RETURN_NAME);
    }
    else {
        mvwaddwstr(file_panel->panel, y, x, buffer);
    }
    if(is_free) {
        free(buffer);
    }
}

void print_tab_space(File_Panel *file_panel) {
    int height, width;
    TAB_SPACE_SIZE(file_panel->panel, height, width);

    wchar_t HLINE = U_HLINE;
    for(int i = 1; i < width; i++) {
        mvwaddnwstr(file_panel->panel, height, i, &HLINE, 1);
    }

    mvwaddwstr(file_panel->panel, 0, 1, TAB_NAME);

    print_current_directory(file_panel, false);
}

void print_current_directory(File_Panel *file_panel, bool with_clear) {
    int row, column;
    TAB_SPACE_POS(row, column);

    if(with_clear) {
        clear_y_str(file_panel->panel, row, 1, -1);
    }

    bool is_free = false;
    wchar_t* buffer = file_panel->current_directory;
    if(wcslen(buffer) > getmaxx(file_panel->panel) - 4) {
        buffer = standart_abreviated(buffer, getmaxx(file_panel->panel) - 4);
    }
    mvwaddwstr(file_panel->panel, row, column, buffer);

    if(is_free) {
        free(buffer);
    }
}

wchar_t* get_select_file(File_Panel* file_panel) {
    int size = wcslen(file_panel->current_directory) + 2 + file_panel->file_menu->items[file_panel->file_menu->selected_item]->size;
    wchar_t* cur_file = (wchar_t*)calloc(size, sizeof(wchar_t));

    swprintf(cur_file, size, L"%ls/%ls", file_panel->current_directory, 
            file_panel->file_menu->items[file_panel->file_menu->selected_item]->string);

    return cur_file;
}

int del_file(File_Panel* file_panel) {
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
    wrefresh(file_panel->menu_sub_win);

    return status;
}