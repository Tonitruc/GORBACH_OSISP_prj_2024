#include "message_box.h"
//Инициализация формы (высота, ширина, название, сообщение, необходимость подтверждения)
MSG_BOX* init_message_box(int height, int width, wchar_t* title, wchar_t* message, bool is_verified) {
    int start_x, start_y;
    CENTER_SCR(start_x, start_y); //Расположение по центру экрана 
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

void set_color_msg(MSG_BOX* msg_box, short color_pair) {  //Установка цвета сообщения 
    msg_box->color_pair = color_pair;
    wattron(msg_box->window, COLOR_PAIR(color_pair));
    wbkgd(msg_box->window, COLOR_PAIR(color_pair));
}

MSG_REQ print_msg(MSG_BOX* msg, bool only_show) {  //Вывод сообщения 
    MENU* menu = NULL;
    if(msg->is_verified) { //Созднаия пунктов меню для подтвержединя 
        WINDOW* win = derwin(msg->window, 1, msg->width - 4, msg->height - 1, 2);
        MITEM **items = (MITEM**)calloc(3, sizeof(MITEM));
        items[0] = init_menu_item(L"[ ОК ]");
        items[1] = init_menu_item(L"[ ОТМЕНА ]");
        SETTINGS_MENU set_menu = NONE_SPRT | NON_DESIG_ITEMS | USER_COL_SIZE | NON_COL_NAME | ALLIGMENT_CENTER;
        menu = init_menu(items, msg->window, win, GRID, set_menu);
        init_menu_format(menu, 1, 2);
        menu->slctd_item_color_pair = SLCTD_EXCEPTION_COLOR; 
        set_columns_size(menu, (double*)2, 0.5, 0.5);
    }

    int status = M_ALLOW;
    int key;
    mvwaddwstr(msg->window, getmaxy(msg->window) / 2, 2, msg->message);
    box(msg->window, 0, 0);
    if(!msg->is_verified && !only_show) {
        mvwprintw(msg->window, getmaxy(msg->window) - 1, 1, " Нажмите любую клавишу... ");
    }
    mvwaddwstr(msg->window, 0, (msg->width - 2) / 2 - wcslen(msg->title) / 2, msg->title);
    
    wrefresh(msg->window);
    if(!only_show) {
        do {  //Обработчик нажатий сообщения 
        print_menu(menu);
        wrefresh(msg->window);
        if(msg->is_verified) {
            wrefresh(menu->subwin);
        }
        key = wgetch(msg->window);
        if(key == '\n' && msg->is_verified) {  //Если был нажат Enter 
            if(menu->select == 0) {
                status = M_ALLOW;
            } else {
                status = M_CANCEL;
            }
            break;
        } 
        else if (key == KEY_RIGHT && msg->is_verified) { //Обработка клавиш влево вправо 
            menu_driver(menu, REQ_RIGHT_ITEM);
        } 
        else if (key == KEY_LEFT && msg->is_verified) {
            menu_driver(menu, REQ_LEFT_ITEM);
        }
        else if(key == KEY_MOUSE && msg->is_verified) {  //Обработка нажатий мыши
            MEVENT mevent;
            if(getmouse(&mevent) == OK) {
                find_click_item(menu, mevent);
            }
        }
        else if(key == KEY_RESIZE) {
            status = M_CANCEL;
            break;
        }
    } while(msg->is_verified);
    }

    if(msg->is_verified) {
        delwin(menu->subwin);
        free_menu(menu);
    }
    free_msg(msg);

    return status;
}

void free_msg(MSG_BOX* msg) { //Очистка формы 
    free(msg->title);
    free(msg->message);

    delwin(msg->window);
    free(msg);
}