#include "message_box.h"

MSG_BOX* init_message_box(int height, int width, wchar_t* title, wchar_t* message, bool is_verified) {
    int start_x, start_y;
    CENTER_SCR(start_x, start_y);
    start_x -= height / 2;
    start_y -= width / 2;

    MSG_BOX* message_box = (MSG_BOX*)calloc(1, sizeof(MSG_BOX));
    message_box->window = newwin(height, width, start_x, start_y);

    message_box->height = height;
    message_box->width = width;
    message_box->is_verified = is_verified;
    
    message_box->title = (wchar_t*)calloc(wcslen(title + 1), sizeof(wchar_t*));
    message_box->message = (wchar_t*)calloc(wcslen(message + 1), sizeof(wchar_t*));
    wattron(message_box->window, COLOR_PAIR(EXCEPTION_COLOR));
    wbkgd(message_box->window, COLOR_PAIR(EXCEPTION_COLOR));
    keypad(message_box->window, TRUE);
    wcscpy(message_box->title, title);
    wcscpy(message_box->message, message);

    return message_box;
}

void set_color_msg(MSG_BOX* msg_box, short color_pair) {
    msg_box->color_pair = color_pair;
    wattron(msg_box->window, COLOR_PAIR(color_pair));
    wbkgd(msg_box->window, COLOR_PAIR(color_pair));
}

MSG_REQ show_msg(MSG_BOX* msg) {
    MENU* menu;
    if(msg->is_verified) {
        WINDOW* win = derwin(msg->window, 1, msg->width - 4, msg->height - 1, 2);
        keypad(win, TRUE);
        MITEM **items = (MITEM**)calloc(3, sizeof(MITEM));
        items[0] = init_menu_item(L"[ ОК ]");
        items[1] = init_menu_item(L"[ ОТМЕНА ]");

        SETTINGS_MENU set_menu = NONE_SPRT | NON_DESIG_ITEMS | USER_COL_SIZE | NON_COL_NAME;
        menu = init_menu(items, msg->window, win, GRID, set_menu);
        init_menu_format(menu, 1, 2);
        menu->slctd_item_color_pair = SLCTD_EXCEPTION_COLOR; 
        set_columns_size(menu, (double*)2, 1.0/3.0, 2.0/3.0);
    } else {
        mvwprintw(msg->window, getmaxy(msg->window) - 1, 1, " Нажмите любую клавишу ");
    }

    int status;
    int key;
    mvwaddwstr(msg->window, getmaxy(msg->window) / 2, 2, msg->message);
    box(msg->window, 0, 0);
    mvwaddwstr(msg->window, 0, (msg->width - 2) / 2 - wcslen(msg->title) / 2, msg->title);

    int i = 0;
    do {
        print_menu(menu);
        wrefresh(msg->window);
        key = wgetch(msg->window);
        if(key == '\n') {
            if(menu->selected_item == 0) {
                status = ALLOW;
            } else {
                status = CANCEL;
            }
            break;
        } 
        else if (key == KEY_RIGHT) {
            menu_driver(menu, REQ_RIGHT_ITEM);
        } 
        else if (key == KEY_LEFT) {
            menu_driver(menu, REQ_LEFT_ITEM);
        }
        else if(key == KEY_MOUSE && msg->is_verified) {
            MEVENT mevent;
            if(getmouse(&mevent) == OK) {
                find_click_item(menu, mevent);
                wrefresh(menu->sub_window);
            }
        }
    } while(msg->is_verified);

    wclear(msg->window);
    wrefresh(msg->window);
    free_msg(msg);

    return status;
}

void free_msg(MSG_BOX* msg) {
    free(msg->title);
    free(msg->message);

    delwin(msg->window);
    free(msg);
}