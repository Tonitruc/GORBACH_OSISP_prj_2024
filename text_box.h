#pragma once

//____________ Библиотека для создания формы с вводом данных________________

#include <ncurses.h>
#include <stdbool.h>
#include <form.h>
#include "menu.h"
#include "file_types.h"
#include "fwchar.h"
#include "winmanip.h"
#include "file_operation.h"

typedef enum _TEXT_REQ { //Ответ формы 
    BAD_INPUT = 0,
    T_ALLOW = 1, 
    T_CANCEL = -1
} TEXT_REQ;

typedef struct _TEXT_WIN { //Структура для создания формы 
   WINDOW* window; //Род. окно 
   wchar_t* title; //Название формы 
   wchar_t* message; //Сообщение 
   wchar_t* input; //Строка с результатами ввода 
   int height; int width;
   int start_x; int start_y;
   short color_pair;
   const char* pattern;
   bool box;
   bool verify;
   FORM* form; //Поле для ввода 
   FIELD** fields;
   MENU* menu;
} TEXT_BOX;

TEXT_BOX* init_text_box(WINDOW* par_win, int height, int width, int y, int x, wchar_t* title, wchar_t* message, const char* pattern, bool verify);
void set_color_text(TEXT_BOX* tb, short color_pair);
void show_text_box(TEXT_BOX* tb);
TEXT_REQ input_text_box(TEXT_BOX* tb);
void free_text(TEXT_BOX* tb);
void set_start_text(TEXT_BOX* tb, wchar_t* text);
wchar_t* save_input(TEXT_BOX* tb);

