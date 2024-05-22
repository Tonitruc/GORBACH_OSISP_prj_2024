#pragma once

//____________ Библиотека для создания базовой рабочей панели________________

#include "file_panel.h"
#include "fcntl.h"
#include <pwd.h>
#include <grp.h>

typedef enum _PANEL_MODE { //Режимы работы панели 
	FILE_LIST = 0,
	FILE_VIEW = 1,
    FILE_INFO = 2
} PANEL_MODE;

typedef struct _FILE_SCROLL { //Структура для запоминания позиции в файле 
    long beg_pos;
    FILE* file;
    bool eof;
} FSCROLL;

typedef struct _WORK_PANEL { //Рабоая панель 
    FSCROLL scroll;
    FILE_PANEL* fpanel;
    PANEL_MODE mode;

    struct _WORK_PANEL* dep; //Зависимая рабоая панель 
    bool is_select;
} WPANEL;

WPANEL* init_wpanel(FILE_PANEL* file_panel, bool is_select);
void refersh_wpanel(WPANEL* wpanel);
void free_wpanel(WPANEL* wpanel);
WPANEL* change_wpanel(WPANEL* wpanel);
void resize_wpanel(WPANEL* wpanel, int num);
void change_mode(WPANEL* wpanel, PANEL_MODE mode);
void show_file_data(WPANEL* mpanel, WPANEL* spanel, short color);
void print_file_data(WPANEL* wpanel);
void wkeypad_handler(WPANEL* wpanel, int key);
void wmouse_handler(WPANEL* wpanel, MEVENT mevent);
void print_ex_finfo(WPANEL* mpanel, WPANEL* spanel, short color);
