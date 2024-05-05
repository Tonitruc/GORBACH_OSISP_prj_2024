#pragma once

#include <ncurses.h>
#include <stdbool.h>
#include <form.h>
#include "menu.h"
#include "file_types.h"
#include "fwchar.h"
#include "winmanip.h"
#include "file_operation.h"

typedef enum _TEXT_REQ {
    BAD_INPUT = 0,
    T_ALLOW = 1, 
    T_CANCEL = -1
} TEXT_REQ;

typedef TEXT_REQ (*VALIDATOR)(wchar_t* wstr);
typedef int (*FIELD_VALIDATOR)(int sym);

typedef struct _TEXT_WIN {
   WINDOW* window;
   wchar_t* title;
   wchar_t* message;
   wchar_t* input;
   int height; int width;
   int start_x; int start_y;
   short color_pair;
   const char* pattern;
   bool box;
   bool verify;
   FORM* form;
   FIELD** fields;
   MENU* menu;
} TEXT_BOX;

TEXT_BOX* init_text_box(WINDOW* par_win, int height, int width, int y, int x, wchar_t* title, wchar_t* message, const char* pattern, bool verify);
void set_color_text(TEXT_BOX* tb, short color_pair);
void show_text_box(TEXT_BOX* tb);
TEXT_REQ input_text_box(TEXT_BOX* tb, char** result);
void free_text(TEXT_BOX* tb);
