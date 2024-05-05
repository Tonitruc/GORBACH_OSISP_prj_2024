#pragma once

#include "file_panel.h"
#include "fcntl.h"

typedef enum _PANEL_MODE {
	FILE_INFO = 2,
	FILE_LIST = 0,
	FILE_VIEW = 1
} PANEL_MODE;

typedef struct _FILE_SCROLL {
    off_t beg_pos;
    int file;
    bool eof;
} FSCROLL;

typedef struct _WORK_PANEL {
    FSCROLL scroll;
    FILE_PANEL* fpanel;
    PANEL_MODE mode;

    struct _WORK_PANEL* dep;
} WPANEL;

WPANEL* init_wpanel(FILE_PANEL* file_panel);
void refersh_wpanel(WPANEL* wpanel);
void free_wpanel(WPANEL* wpanel);
WPANEL* change_wpanel(WPANEL* wpanel);
void resize_wpanel(WPANEL* wpanel, int num);
void change_mode(WPANEL* wpanel, PANEL_MODE mode);
void show_file_data(FILE_PANEL* mpanel, FILE_PANEL* spanel);
void print_file_data(WINDOW* window, int file);
