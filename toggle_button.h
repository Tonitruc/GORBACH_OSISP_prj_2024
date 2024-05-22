#pragma once

//____________ Библиотека для кнопки состояния________________

#include <wchar.h>
#include <ncurses.h>
#include <stdlib.h>

typedef enum _TBUTTON_STATE {  //Состояние кнопки 
    CHECKED = 1,
    UNCHECKED = -1
} TBUTTON_STATE;

typedef struct _TOGGLE_BUTTON { //Структура для создания кнопки 
    int x;
    int y;

    TBUTTON_STATE state;
    WINDOW* parent;
} TOGGLE_BUTTON;

TOGGLE_BUTTON* create_tbutton(int y, int x, WINDOW* window, wchar_t* text, TBUTTON_STATE base_state);
void refersh_button(TOGGLE_BUTTON* tbutton);
bool check_click_tbutton(TOGGLE_BUTTON* tg, MEVENT* mevent);