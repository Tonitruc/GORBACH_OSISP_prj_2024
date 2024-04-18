#include "text_box.h"

TEXT_BOX* init_text_box(int height, int width, wchar_t* title, wchar_t* message, VALIDATOR validator) {
    int start_x, start_y;
    CENTER_SCR(start_x, start_y);
    start_x -= height / 2;
    start_y -= width / 2;

    TEXT_BOX* text_box = (TEXT_BOX*)calloc(1, sizeof(TEXT_BOX));
    text_box->window = newwin(height, width, start_x, start_y);

    text_box->height = height;
    text_box->width = width;
    text_box->validator = validator;
    
    text_box->title = (wchar_t*)calloc(wcslen(title + 1), sizeof(wchar_t*));
    text_box->message = (wchar_t*)calloc(wcslen(message + 1), sizeof(wchar_t*));
    wattron(text_box->window, COLOR_PAIR(TOP_PANEL_COLOR));
    wbkgd(text_box->window, COLOR_PAIR(TOP_PANEL_COLOR));
    keypad(text_box->window, TRUE);
    wcscpy(text_box->title, title);
    wcscpy(text_box->message, message);

    return text_box;
}

void set_color_text(TEXT_BOX* tb, short color_pair) {
    tb->color_pair = color_pair;
    wattron(tb->window, COLOR_PAIR(color_pair));
    wbkgd(tb->window, COLOR_PAIR(color_pair));
}

TEXT_REQ input_text_box(TEXT_BOX* tb) {
    MENU* menu;
    WINDOW* win = derwin(tb->window, 1, tb->width - 4, tb->height - 1, 2);
    keypad(win, TRUE);

    MITEM **items = (MITEM**)calloc(3, sizeof(MITEM));
    items[0] = init_menu_item(L"[ ОК ]");
    items[1] = init_menu_item(L"[ ОТМЕНА ]");
    SETTINGS_MENU set_menu = NONE_SPRT | NON_DESIG_ITEMS | USER_COL_SIZE | NON_COL_NAME;
    menu = init_menu(items, tb->window, win, GRID, set_menu);
    init_menu_format(menu, 1, 2);
    menu->slctd_item_color_pair = SLCTD_EXCEPTION_COLOR; 
    set_columns_size(menu, (double*)2, 1.0/3.0, 2.0/3.0);

        int start_x, start_y;
    CENTER_SCR(start_x, start_y);
    FIELD *field[2] = {
        new_field(1, tb->width - 4, 0, 0, 0, 0),
        NULL
    }; 
    FORM *form = new_form(field);
    set_form_win(form, tb->window);
    WINDOW* win2 = derwin(tb->window, 1, tb->width - 4, 2, 2);
    set_form_sub(form, win2);
    set_field_opts(field[0], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);

    int status;
    int key;
    mvwaddwstr(tb->window, getmaxy(tb->window) / 2 - 2, 2, tb->message);
    box(tb->window, 0, 0);
    mvwaddwstr(tb->window, 0, (tb->width - 2) / 2 - wcslen(tb->title) / 2, tb->title);
    post_form(form);
    wchar_t sym;

    do {
        print_menu(menu);
                refresh();
        wrefresh(win2);
        wrefresh(tb->window);
        wrefresh(menu->sub_window);
        key = wget_wch(tb->window, &sym);
        if(sym == L'\n' || sym == KEY_ENTER) {
            if(menu->selected_item == 0) {
                status = T_ALLOW;
            } else {
                status = T_CANCEL;
            }
            break;
        } 
        else if (sym == KEY_RIGHT) {
            menu_driver(menu, REQ_RIGHT_ITEM);
        } 
        else if (sym == KEY_LEFT) {
            menu_driver(menu, REQ_LEFT_ITEM);
        }
        else if(sym == KEY_MOUSE) {
            MEVENT mevent;
            if(getmouse(&mevent) == OK) {
                find_click_item(menu, mevent);
            }
        } 
        else if(sym == KEY_BACKSPACE) {
            form_driver(form, REQ_DEL_PREV);
        } 
        else {
            form_driver_w(form, key, sym);
        }

    } while(status != T_ALLOW);

    wclear(tb->window);
    wrefresh(tb->window);
    free_text(tb);

    return status;
}

void free_text(TEXT_BOX* tb) {
    free(tb->title);
    free(tb->message);

    delwin(tb->window);
    free(tb);
}