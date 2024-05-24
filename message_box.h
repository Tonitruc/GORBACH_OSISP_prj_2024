#pragma once

//____________ Библиотека для создания формы с сообщением________________

#include <ncurses.h>
#include <stdbool.h>
#include "menu.h"
#include "file_types.h"
#include "fwchar.h"
#include "winmanip.h"
#include "file_operation.h"

typedef enum _MSG_REQ {  //Ответ формы 
    M_ALLOW = 1, 
    M_CANCEL = -1
} MSG_REQ;

typedef struct _MSG_WIN {  //Структура для создания формы 
   WINDOW* window;
   wchar_t* title;
   wchar_t* message;
   int height; int width;
   short color_pair;
   bool is_verified;
} MSG_BOX;

MSG_BOX* init_message_box(int height, int width, wchar_t* title, wchar_t* message, bool is_verified);
void set_color_msg(MSG_BOX* msg_box, short color_pair);

#define show_msg(msg) print_msg(msg, false);

MSG_REQ print_msg(MSG_BOX* msg, bool only_show);
void free_msg(MSG_BOX* msg);
