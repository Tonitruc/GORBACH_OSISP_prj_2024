#include "menu.h"
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "winmanip.h"

//F_HOBX/GRID types
//-------------Menu items-------------

MITEM* init_menu_item(const wchar_t* string) {
    MITEM* item = (MITEM*)calloc(1, sizeof(MITEM));

    size_t str_len = wcslen(string) + 1;
    item->string = (wchar_t*)calloc(str_len, sizeof(wchar_t));
    wcsncpy(item->string, string, str_len);

    item->size = str_len;
    item->action = NULL;
    item->color = -1;
    item->pnt_sym = 0;
    
    return item;
}   

size_t size_items(MITEM** items) {
    size_t count = 0;
    if(items == NULL)
        return count;

    while(items[count] != NULL) {
        count++;
    }

    return count;
}

//-------------Menu Settings-------------

//*********Help function for init menu****************

bool init_menu_win(MENU* menu, WINDOW* pwin) {
    if(pwin == NULL || menu == NULL)
        return false;

    menu->parent_window = pwin;
    return true;
}

bool init_menu_sub(MENU* menu, WINDOW* subwin) {
    if(subwin == NULL || menu == NULL)
        return false;
    //scrollok(subwin, true);
    //wsetscrreg(subwin, 1, getmaxx(subwin));

    menu->sub_window = subwin;
    return true;
}

//*********For set menu sym****************

void set_sprt_sym(MENU* menu, wchar_t sprt_sym) {
    if(sprt_sym == 0) {
        menu->sprt_sym = BASE_SEPARATE_SYM;
    } 
    else {
        menu->sprt_sym = sprt_sym;
    }
}

void set_slct_sym(MENU* menu, chtype slct_sym) {
    if(slct_sym == 0) {
        menu->slct_sym = BASE_SELECT_SYM;
    }

    menu->slct_sym = slct_sym;
}

void set_pnt_sym(MENU* menu, chtype pnt_sym) {
    if(pnt_sym == 0) {
        menu->pnt_sym = BASE_POINT_SYM;
    } 
    else {
        menu->pnt_sym = pnt_sym;
    }
}

void init_color_slctd_item(MENU* menu, short color_pair) {
    menu->slctd_item_color_pair = color_pair;
}

//*********For init menu sym****************

MENU* init_menu(MITEM** items, WINDOW* p_win, WINDOW* sub_win, TYPE_MENU type, SETTINGS_MENU set_menu) {
    MENU* menu = (MENU*)calloc(1, sizeof(MENU));
    if(menu == NULL) {
        return NULL;
    }
    if(!init_menu_win(menu, p_win)) {
        return NULL;
    }
    if(!init_menu_sub(menu, sub_win)) {
        return NULL;
    }

    menu->type_menu = type;
    menu->set_menu = set_menu;

    menu->items = items;                        //Should contain null in the end
    menu->amount_items = size_items(items);
    menu->items_action = NULL;

    menu->max_rows = BASE_ROW_AMOUNT;           //Set base grid
    menu->rows = BASE_ROW_AMOUNT;
    menu->columns = BASE_COL_AMOUNT;
    menu->selected_item = BASE_SELECTED_ITEM;  

    menu->sprt_sym = BASE_SEPARATE_SYM;

    menu->slct_sym = BASE_SELECT_SYM;
    menu->pnt_sym = BASE_POINT_SYM;
    
    menu->columns_size = NULL;
    menu->column_names = NULL;

    menu->slctd_item_color_pair = 0;

    return menu;     
}

//num zero for all items and items num in menu for some item
void set_item_action(MENU* menu, int num, MIACTION action) {
    if(num == 0) {
        menu->items_action = action;
    }
    else if(num > 0 && num <= menu->amount_items) {
        menu->items[menu->selected_item]->action = action;
    }
}

//Required function after initialization for init format of menu
bool init_menu_format(MENU* menu, int row, int column) {  //row - max row for print column - num of column for print
    if(row <= 0 || column <= 0)
        return false;
    menu->max_rows = row;
    menu->columns = column;
    menu->rows = menu->amount_items / column + 1;  //rows required for output
    if(menu->set_menu & STNDRT_COL_SIZE) {
        set_columns_size(menu, 0);
    }
    if(menu->set_menu & NON_COL_SIZE) {
        calc_item_coord(menu);
    }

    return true;
}

//if set USR_COL_SIZE should be execute
void set_columns_size(MENU *menu, double* col_len, ...) {
    if(col_len == 0) {
        menu->columns_size = (double*)calloc(menu->columns + 1, sizeof(double));
        for(int i = 0; i < menu->columns; i++) {
            menu->columns_size[i] = 1.0 / menu->columns;
        }
    }
    else {
        size_t num_columns = (size_t)col_len;
        va_list args;
        va_start(args, col_len);
        menu->columns_size = (double*)calloc(num_columns + 1, sizeof(double));
        for(int i = 0; i < num_columns; i++) {
            menu->columns_size[i] = va_arg(args, double);
        }
        va_end(args);
    }

    if(menu->set_menu & NON_COL_NAME) {
        calc_item_coord(menu);
    }

}

//if set USE_COL_NAME should be execute
void set_column_name(MENU *menu, const wchar_t* names, ...) {
    menu->column_names = (MITEM**)calloc(menu->columns + 1, sizeof(MITEM*));
    va_list args;
    va_start(args, names);
    for(int i = 0; i < menu->columns; i++) {
        menu->column_names[i] = (MITEM*)malloc(sizeof(MITEM));
        menu->column_names[i]->size = wcslen(names) + 1;
        menu->column_names[i]->string = (wchar_t*)calloc(menu->column_names[i]->size , sizeof(wchar_t));
        wcsncpy(menu->column_names[i]->string, names, menu->column_names[i]->size );
        names = va_arg(args, const wchar_t*);
        menu->column_names[i]->y = 0;
    }
    va_end(args);

    menu->max_rows -= 1;
    calc_item_coord(menu);
}

//F_HOBX/GRID types
void calc_item_coord(MENU* menu) {
    size_t start_x; int start_y;
    int n_item;
    if(menu->selected_item < menu->max_rows * menu->columns) {   //if not all items can show in one page
        start_y = n_item = 0;
    }
    else if(menu->amount_items > menu->max_rows * menu->columns && menu->selected_item >= menu->max_rows * menu->columns) {
        start_y = -1 - (menu->selected_item - menu->max_rows * menu->columns) / menu->columns;
        n_item = 0;
        mvprintw(0, 0, "%d", start_y);
    }
    bool is_break = false;
    while(!is_break) {
        start_x = 0;

        for(int j = 0; j < menu->columns; j++) {

            menu->items[n_item]->x = start_x;
            menu->items[n_item]->size = wcslen(menu->items[n_item]->string);

            //____COLUMN_FORMAT________
            if(menu->columns_size != NULL) {
                start_x += menu->columns_size[j] * getmaxx(menu->sub_window);
            }

            //____SEPARATE_COLUMN______
            if((menu->set_menu & SPRT_INTERMEDIATE && j != 0) 
                || (menu->set_menu & SPRT_ALL)) {
                menu->items[n_item]->x += 1;
            }

            //____DESIGNED_ELEMENT_____
            if((menu->set_menu & DESIG_ITEMS && (menu->type_menu == GRID)) 
                || ((menu->set_menu & DESIG_ITEMS && (menu->type_menu == F_HBOX)) && (n_item % menu->columns == 0))) {
                menu->items[n_item]->x += 1;
            }

            //____WITHOUT_COLUMN_FORMAT_____
            if(menu->columns_size == NULL) {
                start_x = menu->items[n_item]->x + menu->items[n_item]->size;
            } 

            //____COLUMN_NAMES_____
            menu->items[n_item]->y = start_y; 
            if(menu->column_names != NULL) {
                menu->items[n_item]->y += 1;
            }
            n_item++;
            if(n_item == menu->amount_items) {
                is_break = true;
                break;
            }
        }
        start_y++;
    } 
}

//resize menu, should execute if subwin was resize of remove
bool resize_menu(MENU* menu) {
    menu->max_rows = getmaxy(menu->sub_window);
    if(menu->set_menu & USE_COL_NAME) {
        menu->max_rows -= 1;
    }

    calc_item_coord(menu);
    print_menu(menu);

    return true;
} 

//-------------Print Menu-------------

void print_column_names(MENU *menu) {  
    int start_x = 0;
    int size_col = 0;

    for(int i = 0; i < menu->columns; i++) {
        size_col = menu->columns_size[i] * getmaxx(menu->sub_window);
        mvwaddwstr(menu->sub_window, menu->column_names[i]->y, start_x + (size_col - menu->column_names[i]->size) / 2 + 1, 
            menu->column_names[i]->string);
        start_x += size_col;
        if((menu->set_menu & SPRT_INTERMEDIATE && i != menu->columns - 1) ||
            menu->set_menu & SPRT_ALL)
            mvwaddnwstr(menu->sub_window, 0, start_x, &menu->sprt_sym, 1);
    }
}

//new
void print_menu(MENU* menu) {
    if(menu->set_menu & USE_COL_NAME) {
        print_column_names(menu);
    }

    int n_item;
    for(int i = 0; i < menu->amount_items; i++) {
        if((menu->set_menu & NON_COL_NAME && menu->items[i]->y == 0 )
            || (menu->set_menu & USE_COL_NAME && menu->items[i]->y == 1)) {
            n_item = i;
            break;
        }
    }

    bool is_break = false;
    size_t column_size;
    for(int i = 0; i < menu->rows && !is_break; i++) {
        column_size = 0;
        for(int j = 0; j < menu->columns; j++) {
            if((menu->set_menu & SPRT_INTERMEDIATE && j != 0) 
                || (menu->set_menu & SPRT_ALL)) {
                mvwaddnwstr(menu->sub_window, menu->items[n_item]->y, column_size, &menu->sprt_sym, 1);
            }
            if(menu->columns_size != NULL) {
                column_size += menu->columns_size[j] * getmaxx(menu->sub_window);
            }
            else {
                column_size = menu->items[n_item]->x + menu->items[n_item]->size;
            } 

            if(n_item == menu->selected_item && menu->type_menu == GRID) {
                if(menu->set_menu & DESIG_ITEMS){
                    mvwaddch(menu->sub_window, menu->items[n_item]->y, menu->items[n_item]->x - 1, 
                        menu->slct_sym | COLOR_PAIR(menu->slctd_item_color_pair));
                }
                wattron(menu->sub_window, COLOR_PAIR(menu->slctd_item_color_pair));
                mvwaddwstr(menu->sub_window, menu->items[n_item]->y , menu->items[n_item]->x, menu->items[n_item]->string);
                wattroff(menu->sub_window, COLOR_PAIR(menu->slctd_item_color_pair));
            } 
            else {
                if((menu->set_menu & DESIG_ITEMS  && (menu->type_menu == GRID)) 
                    || (menu->set_menu & DESIG_ITEMS  && (menu->type_menu == F_HBOX) && n_item % menu->columns == 0)) {
                    if(menu->items[n_item]->pnt_sym == 0) {
                        mvwaddch(menu->sub_window, menu->items[n_item]->y, menu->items[n_item]->x - 1, menu->pnt_sym);
                    } else {
                        mvwaddch(menu->sub_window, menu->items[n_item]->y, menu->items[n_item]->x - 1, 
                            menu->items[n_item]->pnt_sym | COLOR_PAIR(menu->items[n_item]->color));
                    }
                }

                if(menu->items[n_item]->color == -1) {
                    mvwaddwstr(menu->sub_window, menu->items[n_item]->y , menu->items[n_item]->x, menu->items[n_item]->string);
                } else {
                    mvwaddwstr_color(menu->sub_window, menu->items[n_item]->y , menu->items[n_item]->x, 
                        menu->items[n_item]->string, menu->items[n_item]->color);
                }
            }
            n_item++;
            if(n_item == menu->amount_items) { //bag
                is_break = true;
                break;
            }
        }

        if((menu->type_menu == F_HBOX)) {  //bag
            mvwaddch(menu->sub_window, menu->items[menu->selected_item]->y, menu->items[menu->selected_item]->x - 1, 
                menu->slct_sym | COLOR_PAIR(menu->slctd_item_color_pair));
            menu_str_slctd_color(menu, menu->items[menu->selected_item]->y, false);
        }
    }

}

//todo
void unprint_menu(MENU* menu) {
    werase(menu->sub_window);
}

void menu_driver(MENU* menu, REQ_KEY key) {
    int prev_selected_item = menu->selected_item;
    bool need_scroll = false;
    switch (key)
    {
        case REQ_DOWN_ITEM: { 
            if((menu->selected_item + menu->columns) < menu->amount_items) { 
                menu->selected_item += menu->columns; 
                need_scroll = true; 
            } 
            break;
        }
        case REQ_UP_ITEM: { 
            if(menu->selected_item - menu->columns >= 0) { 
                menu->selected_item -= menu->columns; 
                need_scroll = true; 
            } 
            break; 
        }
        case REQ_LEFT_ITEM: { 
            if(menu->selected_item > 0 && (menu->type_menu == GRID)) { 
                menu->selected_item--; 
                need_scroll = true; 
            } 
            break;
        }
        case REQ_RIGHT_ITEM: { 
            if(menu->selected_item < menu->amount_items - 1 && (menu->type_menu == GRID)) { 
                menu->selected_item++; 
                need_scroll = true; 
            } 
            break; 
        }
        default: break;
    }
    if(need_scroll) {
        change_menu_action(menu, key, prev_selected_item);
        wrefresh(menu->sub_window);
    }
}


void scroll_menu(MENU* menu, REQ_KEY key) {
    scrollok(menu->sub_window, true);
    switch (key)
    {
        case REQ_DOWN_ITEM: {
            wsetscrreg(menu->sub_window, 1, menu->max_rows);
            
            offset_y_items(menu, -1);
            wscrl(menu->sub_window, 1);
            break; 
        }
        case REQ_UP_ITEM: {
            offset_y_items(menu, 1);
            wscrl(menu->sub_window, -1);
            break;
        }
        default: break;
    }
    scrollok(menu->sub_window, false);

}

//todo
void change_menu_action(MENU* menu, REQ_KEY key, int prev_selected_item) {
    bool is_scroll = false;
     
    switch (key)
    { 
    case REQ_DOWN_ITEM: {
        if(menu->items[menu->selected_item]->y == menu->max_rows + 1) {
            scroll_menu(menu, key);
            refresh();
            is_scroll = true;
        }
        break;
    }
    case REQ_UP_ITEM: {
        if(menu->items[menu->selected_item]->y == -1 && menu->set_menu & NON_COL_NAME) {
            scroll_menu(menu, key);
        }
        else if(menu->items[menu->selected_item]->y == 0 && menu->set_menu & USE_COL_NAME) {
            scroll_menu(menu, key);
        }
        is_scroll = true; 
        break;
    }
    default: break; 
    }

    int column_size = 0;
    if((menu->type_menu == F_HBOX)) {
        clear_y_str(menu->sub_window, menu->items[prev_selected_item]->y, 0);
        for(int i = 0; i < menu->columns; i++) {
            if((menu->set_menu & SPRT_INTERMEDIATE && i != 0)
                || (menu->set_menu & SPRT_ALL)) {
                mvwaddnwstr(menu->sub_window, menu->items[menu->selected_item]->y, column_size, &menu->sprt_sym, 1);
                mvwaddnwstr(menu->sub_window, menu->items[prev_selected_item]->y, column_size, &menu->sprt_sym, 1);
            }
            column_size += menu->columns_size[i] * getmaxx(menu->sub_window);
            mvwaddwstr(menu->sub_window, menu->items[menu->selected_item]->y, menu->items[menu->selected_item + i]->x, 
                 menu->items[menu->selected_item + i]->string);
            if(menu->items[prev_selected_item]->color == -1) {
                mvwaddwstr(menu->sub_window, menu->items[prev_selected_item]->y, menu->items[prev_selected_item + i]->x,    //bag
                    menu->items[prev_selected_item + i]->string);
            } else {
                mvwaddwstr_color(menu->sub_window, menu->items[prev_selected_item]->y, menu->items[prev_selected_item + i]->x,    //bag
                    menu->items[prev_selected_item + i]->string, menu->items[prev_selected_item + i]->color);
            }
        }   
        menu_str_slctd_color(menu, menu->items[menu->selected_item]->y, false);
        //menu_str_slctd_color(menu, menu->items[prev_selected_item]->y, true);
    }
    else {
        for(int i = 0; i < menu->columns && is_scroll; i++) {
            if((menu->set_menu & SPRT_INTERMEDIATE && i != 0) 
                || (menu->set_menu & SPRT_ALL)) {
                mvwaddnwstr(menu->sub_window, menu->items[menu->selected_item]->y, column_size, &menu->sprt_sym, 1);
            }
            column_size += menu->columns_size[i] * getmaxx(menu->sub_window);
        mvwaddwstr(menu->sub_window, menu->items[menu->selected_item]->y, 
                    menu->items[menu->selected_item + i - menu->selected_item % menu->columns]->x, 
                     menu->items[menu->selected_item + i - menu->selected_item % menu->columns]->string);
        }   
        mvwaddwstr(menu->sub_window, menu->items[prev_selected_item]->y, menu->items[prev_selected_item]->x, 
                 menu->items[prev_selected_item]->string);
                 
        wattron(menu->sub_window, COLOR_PAIR(menu->slctd_item_color_pair));
        mvwaddwstr(menu->sub_window, menu->items[menu->selected_item]->y, menu->items[menu->selected_item]->x, 
                 menu->items[menu->selected_item]->string);
        wattroff(menu->sub_window, COLOR_PAIR(menu->slctd_item_color_pair));
    }

    if(menu->set_menu & DESIG_ITEMS ) {
        mvwaddch(menu->sub_window, menu->items[prev_selected_item]->y, menu->items[prev_selected_item]->x - 1, menu->pnt_sym);
        mvwaddch(menu->sub_window, menu->items[menu->selected_item]->y, menu->items[menu->selected_item]->x - 1, 
            menu->slct_sym | COLOR_PAIR(menu->slctd_item_color_pair));
    } 
}

void menu_str_slctd_color(MENU* menu, int y_str, bool is_reversed) {
    int cols_len = getmaxx(menu->sub_window);
    wchar_t *wstr = (wchar_t *)calloc(cols_len + 1, sizeof(wchar_t));
    mvwinnwstr(menu->sub_window, y_str, 0, wstr, cols_len);

    clear_y_str(menu->sub_window, y_str, 0);
    if(!is_reversed) {
        wattron(menu->sub_window, COLOR_PAIR(menu->slctd_item_color_pair));
        mvwaddwstr(menu->sub_window, y_str, 0, wstr);
        wattroff(menu->sub_window, COLOR_PAIR(menu->slctd_item_color_pair));
    }
    else { 
        if(menu->items[menu->selected_item]->color != -1) {
            mvwaddwstr_color(menu->sub_window, y_str, 0, wstr, menu->items[menu->selected_item]->color);
        } else {
            mvwaddwstr(menu->sub_window, y_str, 0, wstr);
        }
    }
}

//offset all element by vertical
void offset_y_items(MENU* menu, int y) {
    int n_item = 0;
    bool is_break = false;
    while(!is_break) {
        for(int j = 0; j < menu->columns; j++) {
            menu->items[n_item++]->y += y; 
            if(n_item == menu->amount_items) {
                is_break = true;
                break;
            }
        }
    } 
}

bool find_click_item(MENU* menu, MEVENT event) {
    if(!wmouse_trafo(menu->sub_window, &event.y, &event.x, FALSE)) {
        return false;
    }
    int prev_selected_item = menu->selected_item;
    if(menu->type_menu == F_HBOX) {
        for(int i = 0; i < menu->amount_items; i += menu->columns) {
            if(event.y == menu->items[i]->y) {
                menu->selected_item = i;
                break;
            }
        }
    }
    else if(menu->type_menu == GRID) {
        for(int i = 0; i < menu->amount_items; i++) {
            if(event.y == menu->items[i]->y && event.x >= menu->items[i]->x && event.x < menu->items[i]->x + menu->items[i]->size) {
                menu->selected_item = i;
                break;
            }
        }
    }
    if(prev_selected_item == menu->selected_item) {
        return false;
    }

    change_menu_action(menu, NON_REQ, prev_selected_item);
    wrefresh(menu->sub_window);
    return true;
}

void slctd_item_action(MENU* menu) {
    if(menu->items[menu->selected_item]->action != NULL) {
        menu->items[menu->selected_item]->action();
    } else if(menu->items_action != NULL) {
        menu->items_action();
    }
}

void free_item(MITEM* item) {
    free(item->string);
    free(item);
}

void free_items(MITEM** items) {
    for(int i = 0; items[i] != NULL; i++) {
        free_item(items[i]);
    }
    free(items);
}

void set_new_items(MENU* menu, MITEM** new_items) {
    free_items(menu->items);

    menu->amount_items = size_items(new_items);
    menu->items = new_items;
    menu->selected_item = BASE_SELECTED_ITEM;
    menu->rows = menu->amount_items / menu->columns + 1;

    calc_item_coord(menu);
}

void free_menu(MENU *menu) {
    free_items(menu->items);
    free(menu->columns_size);

    free_items(menu->column_names);

    free(menu);
}