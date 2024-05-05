#include "wpanel.h"

WPANEL* init_wpanel(FILE_PANEL* file_panel) {
    WPANEL* wpanel = (WPANEL*)calloc(1, sizeof(WPANEL));
    wpanel->fpanel = file_panel;
    wpanel->mode = FILE_LIST;
    wpanel->dep = NULL;
    wpanel->scroll.beg_pos = 0;
    wpanel->scroll.file = -1;
    wpanel->scroll.eof = false;

    return wpanel;
}

void refersh_wpanel(WPANEL* wpanel) {
    if(wpanel->mode == FILE_LIST) {
        refresh_file_panel(wpanel->fpanel);
    } 
    else if(wpanel->mode == FILE_VIEW) {
        show_file_data(wpanel->fpanel, wpanel->dep->fpanel);
    }
}

WPANEL* change_wpanel(WPANEL* wpanel) {
    char* buffer = wchtochs(wpanel->fpanel->current_directory);
    chdir(buffer);
    free(buffer);
    return wpanel->dep;

}

void resize_wpanel(WPANEL* wpanel, int num) {
    if(wpanel->mode == FILE_LIST) {
        resize_file_panel(wpanel->fpanel, num);
    } 
    else if(wpanel->mode == FILE_VIEW) {
        wpanel->scroll.beg_pos = 0;
    }
}

void change_mode(WPANEL* wpanel, PANEL_MODE mode) {
    if(mode == FILE_LIST) {
        wpanel->mode = FILE_LIST;
    }
    else if(mode == FILE_VIEW && wpanel->dep->mode == FILE_LIST) {
        wpanel->mode = FILE_VIEW;
    }
}

void free_wpanel(WPANEL* wpanel) {

}

void show_file_data(FILE_PANEL* mpanel, FILE_PANEL* spanel) {
	wchar_t* file_name;
	file_name = mpanel->file_menu->iselect->string;

	int file = open(wchtochs(file_name), O_RDONLY);
	werase(spanel->panel);
	if(file == -1) {
		mvwprintw(spanel->panel, 1, 1, "Не удалось открыть файл %ls", file_name);
	} else {
		print_file_data(spanel->panel, file);
	}

	box(spanel->panel, 0, 0);
}

void scroll_file_view(WPANEL* wpanel, int key) {
    char* buffer = wchtochs(wpanel->fpanel->file_menu->iselect->string);
    int file = open(buffer, O_RDONLY);
    lseek(file, wpanel->scroll.beg_pos, SEEK_SET);
    off_t offset = 0;
    char sym;
    if(key == KEY_DOWN && !wpanel->scroll.eof) {
        while(read(file, &sym, 1) != '\n' && offset < getmaxx(wpanel->fpanel->panel) - 2) {
            offset++;
        }
        offset++;
    } 
    else if(key == KEY_UP && wpanel->scroll.beg_pos != 0) {
        lseek(file, -1, SEEK_CUR);
        while(read(file, &sym, 1) != '\n' && -offset < getmaxx(wpanel->fpanel->panel) - 2) {
            offset--;
            lseek(file, -2, SEEK_CUR);
        }
        offset--;
    }
    wpanel->scroll.beg_pos += offset;
    close(file);
}

void print_file_data(WINDOW* window, int file) {
	char buffer;

	for(int y = 1; y < getmaxy(window) - 1; y++) {
		for(int x = 1; x < getmaxx(window) - 1; x++) {
			if((read(file, &buffer, 1)) == 0) {
				return;
			} 

			mvwprintw(window, y, x, "%c", buffer);
			if(buffer == '\n') {
				break;
			}
		}
	}
}