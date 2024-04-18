#pragma once

#include <ncursesw/ncurses.h>
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

typedef struct _TEXT_WIN {
   WINDOW* window;
   wchar_t* title;
   wchar_t* message;
   wchar_t* input;
   int height; int width;
   short color_pair;
   VALIDATOR validator;
} TEXT_BOX;

TEXT_BOX* init_text_box(int height, int width, wchar_t* title, wchar_t* message, VALIDATOR validator);
void set_color_text(TEXT_BOX* tb, short color_pair);
TEXT_REQ input_text_box(TEXT_BOX* tb);
void free_text(TEXT_BOX* tb);
