#include "toggle_button.h"
//Создание кнопки (расположение, окно, текст, стартовое состояние )
TOGGLE_BUTTON* create_tbutton(int y, int x, WINDOW* window, wchar_t* text, TBUTTON_STATE base_state) {
    TOGGLE_BUTTON* tb = (TOGGLE_BUTTON*)calloc(1, sizeof(TOGGLE_BUTTON));
    tb->state = base_state;
    tb->parent = window;
    tb->x = x;
    tb->y = y;

    mvwprintw(window, y, x - 1, "[ ]");
    if(base_state == CHECKED) {
        mvwprintw(window, y, x, "X");
    }

    mvwprintw(window, y, x + 2, "%ls", text);

    return tb;
}
//Обновление состояния кнопки 
void refersh_button(TOGGLE_BUTTON* tbutton) {
    if(tbutton->state == UNCHECKED) {
        tbutton->state = CHECKED;
        mvwprintw(tbutton->parent, tbutton->y, tbutton->x, "X");
    } else {
        tbutton->state = UNCHECKED;
        mvwprintw(tbutton->parent, tbutton->y, tbutton->x, " ");
    }
} 
//Проверка была ли она нажата 
bool check_click_tbutton(TOGGLE_BUTTON* tg, MEVENT* mevent) {
    return mevent->x == tg->x && mevent->y == tg->y; 
}