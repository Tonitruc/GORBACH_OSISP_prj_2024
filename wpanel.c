#include "wpanel.h"

WPANEL* init_wpanel(FILE_PANEL* file_panel, bool is_select) {
    WPANEL* wpanel = (WPANEL*)calloc(1, sizeof(WPANEL));
    wpanel->fpanel = file_panel;
    wpanel->mode = FILE_LIST;
    wpanel->dep = NULL;
    wpanel->scroll.beg_pos = 0;
    wpanel->scroll.file = -1;
    wpanel->scroll.eof = false;
    wpanel->is_select = is_select;

    return wpanel;
}

void refersh_wpanel(WPANEL* wpanel) {
    short color = wpanel->is_select? MENU_WHITE : MENU_GRAY;
    if(wpanel->mode == FILE_LIST) {
            refresh_file_panel(wpanel->fpanel, color);
    } 
    else if(wpanel->mode == FILE_VIEW) {
            show_file_data(wpanel->dep, wpanel, color);
        wrefresh(wpanel->fpanel->panel);
    } 
    else if(wpanel->mode == FILE_INFO) {
        print_ex_finfo(wpanel->dep, wpanel, color);
        wrefresh(wpanel->fpanel->panel);
    }
}

WPANEL* change_wpanel(WPANEL* wpanel) {
    if(wpanel->dep->mode == FILE_LIST) {
        char* buffer = wchtochs(wpanel->dep->fpanel->current_directory);
        chdir(buffer);
        free(buffer);
    } 
    wpanel->dep->is_select = true;
    wpanel->is_select = false;
    return wpanel->dep;

}

void resize_wpanel(WPANEL* wpanel, int num) {
    if(wpanel->mode == FILE_LIST) {
        resize_file_panel(wpanel->fpanel, num);
    } 
    else if(wpanel->mode == FILE_VIEW) {
        resize_file_panel(wpanel->fpanel, num);
        wpanel->scroll.beg_pos = 0;
    }
    else if(wpanel->mode == FILE_INFO) {
        resize_file_panel(wpanel->fpanel, num);
    }
}

void change_mode(WPANEL* wpanel, PANEL_MODE mode) {
    if(mode == FILE_LIST) {
        wpanel->mode = FILE_LIST;
    }
    else if(mode == FILE_VIEW && wpanel->dep->mode == FILE_LIST) {
        wpanel->mode = FILE_VIEW;
    }
    else if(mode == FILE_INFO && wpanel->dep->mode == FILE_LIST) {
        wpanel->mode = FILE_INFO;
    }
}

void free_wpanel(WPANEL* wpanel) {

}

void show_file_data(WPANEL* mpanel, WPANEL* spanel, short color) {
	wchar_t* file_name;
	file_name = get_select_file(mpanel->fpanel);

	spanel->scroll.file = open(wchtochs(file_name), O_RDONLY);
	werase(spanel->fpanel->panel);
    wattron(spanel->fpanel->panel, COLOR_PAIR(color));
    box(spanel->fpanel->panel, 0, 0);
    wattroff(spanel->fpanel->panel, COLOR_PAIR(color));
	if(spanel->scroll.file == -1) {
		mvwprintw(spanel->fpanel->panel, 1, 1, "Не удалось открыть файл %ls", file_name);
        return;
	} 
    else if(get_n_element(mpanel->fpanel->files_info, mslct(mpanel->fpanel->file_menu))->data->file_type == DIRECTORY) {
		mvwprintw(spanel->fpanel->panel, 1, 1, "Это каталог");
    }
    else {
		print_file_data(spanel);
	}
    mvwprintw(spanel->fpanel->panel, 0, getmaxx(spanel->fpanel->panel) / 2 - 5, " Содержимое %ld", spanel->scroll.beg_pos);
    close(spanel->scroll.file);
}

void scroll_file_view(WPANEL* wpanel, int key) {
    char* buffer = wchtochs(get_select_file(wpanel->dep->fpanel));
    FILE* file = fopen(buffer, "r");
    fseek(file, wpanel->scroll.beg_pos, SEEK_SET);
    char sym; int i = 0;
    if(key == KEY_DOWN && !wpanel->scroll.eof) {
        while((sym = fgetc(file)) != '\n' && i < (getmaxx(wpanel->fpanel->panel) - 2)) {
            i++;
            wpanel->scroll.beg_pos++;
        }
        wpanel->scroll.beg_pos++;
    } 
    else if(key == KEY_UP && wpanel->scroll.beg_pos != 0) {
        fseek(file, -2, SEEK_CUR);
        while((sym = fgetc(file)) != L'\n' && i < getmaxx(wpanel->fpanel->panel) - 2 && wpanel->scroll.beg_pos > 1) {
            wpanel->scroll.beg_pos--;
            i++;
            fseek(file, -2, SEEK_CUR);
        }
        wpanel->scroll.beg_pos--;
    }
    fclose(file);
}

void print_file_data(WPANEL* wpanel) {
	char buffer;

    lseek(wpanel->scroll.file, wpanel->scroll.beg_pos, SEEK_SET);
	for(int y = 1; y < getmaxy(wpanel->fpanel->panel) - 1; y++) {
		for(int x = 1; x < getmaxx(wpanel->fpanel->panel) - 1; x++) {
			if((read(wpanel->scroll.file, &buffer, 1)) == 0) {
                wpanel->scroll.eof = true;
				return;
			} 

			mvwprintw(wpanel->fpanel->panel, y, x, "%c", buffer);
			if(buffer == '\n') {
				break;
			}
		}
	}
    wpanel->scroll.eof = false;
}

void wkeypad_handler(WPANEL* wpanel, int key) {
    if(wpanel->mode == FILE_LIST) {
        WPANEL* dep = wpanel->mode == FILE_LIST && wpanel->dep->mode == FILE_LIST? wpanel->dep : wpanel;
        if(keyboard_event_handler(wpanel->fpanel, key, dep->fpanel)) {
            wpanel->dep->scroll.beg_pos = 0;
            wpanel->dep->scroll.eof = false;
        }
    }
    else if(wpanel->mode == FILE_VIEW) {
        scroll_file_view(wpanel, key);
    }
}

void wmouse_handler(WPANEL* wpanel, MEVENT mevent) {
    if(wpanel->mode == FILE_LIST) {
        if(mouse_event_handler(wpanel->fpanel, mevent)) {
            wpanel->dep->scroll.beg_pos = 0;
            wpanel->dep->scroll.eof = false;
        }
    }
    else if(wpanel->mode == FILE_VIEW) {
        if(mevent.bstate & BUTTON5_PRESSED) {
            scroll_file_view(wpanel, KEY_DOWN);
        } else if(mevent.bstate & BUTTON4_PRESSED) {
            scroll_file_view(wpanel, KEY_UP);
        }
    }
}

void print_ex_finfo(WPANEL* mpanel, WPANEL* spanel, short color) {
    wchar_t* file_name;
	file_name = get_select_file(mpanel->fpanel);
    werase(spanel->fpanel->panel);
    WINDOW* subwin = wbrt_derwin(spanel->fpanel->panel, 1);

    wattron(spanel->fpanel->panel, COLOR_PAIR(color));
    box(spanel->fpanel->panel, 0, 0);
    wattroff(spanel->fpanel->panel, COLOR_PAIR(color));
    mvwprintw(spanel->fpanel->panel, 0, getmaxx(spanel->fpanel->panel) / 2 - 5, " Информация ");

	int fd = open(wchtochs(file_name), O_RDONLY);
    if(fd == -1) {
		mvwprintw(subwin, 0, 0, "Не удалось открыть файл %ls", file_name);
        delwin(subwin);
        return;
    }
    struct stat st;
    if(fstat(fd, &st) == -1) {
        mvwprintw(subwin, 0, 0, "Не удалось получить информацию о %ls", file_name);
    }

    char buffer[20];
    struct tm* time_m;
    struct passwd *user_info;
    struct group *group_info;
    mvwprintw(subwin, 0, 0, "Имя: %ls", mpanel->fpanel->file_menu->iselect->string);
    mvwprintw(subwin, 1, 0, "Идентификатор: %ld", st.st_ino);
    mvwprintw(subwin, 2, 0, "Права: %d", st.st_mode);
    mvwprintw(subwin, 3, 0, "Количество жестких ссылок: %ld", st.st_nlink);
    mvwprintw(subwin, 4, 0, "Размер: %ld (%ld блоков)", st.st_size, st.st_blocks);
    time_m = localtime(&st.st_atime);
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", time_m);
    mvwprintw(subwin, 5, 0, "Время доступа: %s", buffer);
    time_m = localtime(&st.st_mtime);
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", time_m);
    mvwprintw(subwin, 6, 0, "Время модифицирования: %s", buffer);
    time_m = localtime(&st.st_ctime);
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", time_m);
    mvwprintw(subwin, 7, 0, "Время создания: %s", buffer);
    user_info = getpwuid(st.st_uid);
    group_info = getgrgid(st.st_gid);
    mvwprintw(subwin, 8, 0, "Владелец: %s/%s", user_info->pw_name, group_info->gr_name);


    close(fd);
    delwin(subwin);
}