#include "menu.h"
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "winmanip.h"

//-------------Функции для работы с элементами меню-------------
//Инициализация элемента меню
MITEM* init_menu_item(const wchar_t* string) {
    MITEM* item = (MITEM*)calloc(1, sizeof(MITEM));

    size_t str_len = wcslen(string) + 1;
    item->string = (wchar_t*)calloc(str_len, sizeof(wchar_t));
    wcsncpy(item->string, string, str_len);

    item->size = str_len;
    item->action = NULL;
    item->color = -1;
    item->pnt_sym = 0;
    item->pnt_color = -1;
    item->slct_color = -1;
    item->sprt_color = -1;
    
    return item;
}   
//Возврат размера элеметов
size_t size_items(MITEM** items) {
    size_t count = 0;
    if(items == NULL)
        return count;

    while(items[count] != NULL) {
        count++;
    }

    return count;
}
//Проверка вхождения элемента в массив  
bool mcontains(MENU* menu, const wchar_t* string) {
    for(int i = 0; i < menu->size; i++) {
        if(wcscmp(menu->items[i]->string, string) == 0) {
            return true;
        }
    }

    return false;
}
//Поиск элемента в массиве элементов 
int find_mitem(MITEM** items, wchar_t* string) {
    size_t size = size_items(items);

    for(int i = 0; i < size; i++) {
        if(wcscmp(items[i]->string, string) == 0) {
            return i;
        }
    }

    return -1;
}
//Добавление элемента в меню 
void add_item(MENU* menu, MITEM* new_item, int index) {
    menu->items = (MITEM**)realloc(menu->items, (menu->size + 2) * sizeof(MITEM*));
    menu->group = (MITEM**)realloc(menu->group, (menu->size + 2) * sizeof(MITEM*));
    clear_group(menu);
    for(int i = menu->size; i >= index; i--) {
        menu->items[i + 1] = menu->items[i];
    }
    menu->items[index] = new_item;
    if(menu->select == index) {
        menu->iselect = menu->items[index];
    }

    menu->size++;
    calc_item_coord(menu);
}
//Удаление элемента из меню 
void delete_item(MENU* menu, int index) {
    for(int i = index; i < menu->size; i++) {
        menu->items[i] = menu->items[i + 1];
    }
    menu->items = (MITEM**)realloc(menu->items, menu->size * sizeof(MITEM*));
    menu->group = (MITEM**)realloc(menu->group, menu->size * sizeof(MITEM*));
    clear_group(menu);

    menu->size--;
    calc_item_coord(menu);
}
//Возврат номера выбранного элемента 
int mslct(MENU* menu) {
    if(menu->type == F_HBOX) {
        return menu->select / menu->columns;
    } else {
        return menu->select;
    }
}

//-------------Функции для работы с группами элементов в меню-------------
//Проверка есть ли элементы в группе 
bool is_clear_group(MENU* menu) {
    bool is_clear = true;
    for(int i = 0; i < menu->size; i++) {
        if(menu->group[i] != NULL) {
            return false;
        }
    }

    return is_clear;
}
//Добавление или удаление из группы элементов 
void group_act(MENU* menu, int index) {
    if(menu->group[index] == NULL) {
        menu->group[index] = menu->items[index];
    } else {
        menu->group[index] = NULL;
    }
}
//Очистка группы 
void clear_group(MENU* menu) {
    for(int i = 0; i < menu->size; i++) {
        menu->group[i] = NULL;
    }
}
//Возврат размера группы 
int group_size(MENU* menu) {
    int size = 0;
    for(int i = 0; i < menu->size; i++) {
        if(menu->group[i] != NULL) {
            size++;
        }
    }

    return size;
}

//-------------Настройки меню-------------
//Установка родительского и дочернего окна для меню 
bool init_menu_win(MENU* menu, WINDOW* pwin) {
    if(pwin == NULL || menu == NULL)
        return false;

    menu->parwin = pwin;
    return true;
}

bool init_menu_sub(MENU* menu, WINDOW* subwin) {
    if(subwin == NULL || menu == NULL)
        return false;

    keypad(menu->subwin, TRUE);
    menu->subwin = subwin;
    return true;
}

//*********Для установки символов обозначения пунктов меню****************

void set_sprt_sym(MENU* menu, wchar_t sprt_sym) {
    if(sprt_sym == 0) {
        menu->sprt_sym = BASE_SEPARATE_SYM;
    } else {
        menu->sprt_sym = sprt_sym;
    }
}

void set_slct_sym(MENU* menu, chtype slct_sym) {
    if(slct_sym == 0) {
        menu->slct_sym = BASE_SELECT_SYM;
    } else {
        menu->slct_sym = slct_sym;
    }
}

void set_pnt_sym(MENU* menu, chtype pnt_sym) {
    if(pnt_sym == 0) {
        menu->pnt_sym = BASE_POINT_SYM;
    } else {
        menu->pnt_sym = pnt_sym;
    }
}

void init_color_slctd_item(MENU* menu, short color_pair) {
    menu->slctd_item_color_pair = color_pair;
}

//*********Создание меню****************

MENU* init_menu(MITEM** items, WINDOW* p_win, WINDOW* sub_win, TYPE_MENU type, SETTINGS_MENU set_menu) {
    MENU* menu = (MENU*)calloc(1, sizeof(MENU));
    if(!init_menu_win(menu, p_win)) {
        return NULL;
    }
    if(!init_menu_sub(menu, sub_win)) {
        return NULL;
    }

    menu->type = type;
    menu->set_menu = set_menu;

    menu->items = items;                        
    menu->size = size_items(items);
    menu->items_action = NULL;

    menu->max_rows = BASE_ROW_AMOUNT;           
    menu->rows = BASE_ROW_AMOUNT;
    menu->columns = BASE_COL_AMOUNT;
    menu->select = BASE_SELECTED_ITEM;  
    menu->iselect = menu->items[menu->select];

    menu->sprt_sym = BASE_SEPARATE_SYM;

    menu->slct_sym = BASE_SELECT_SYM;
    menu->pnt_sym = BASE_POINT_SYM;
    
    menu->columns_size = NULL;
    menu->column_names = NULL;
    menu->is_sort = false;
    menu->items_abr = NULL;
    menu->max_column = getmaxx(menu->subwin);
    menu->div_static_size = true;

    menu->slctd_item_color_pair = 0;

    menu->group = (MITEM**)calloc(menu->size + 1, sizeof(MITEM*));
    menu->group_color = MENU_GROUP;

    return menu;     
}
//Установка функции действия при нажатии на элемет меню
void set_item_action(MENU* menu, int num, MIACTION action) {
    if(num == 0) {
        menu->items_action = action;
    }
    else if(num > 0 && num <= menu->size) {
        menu->items[num]->action = action;
    }
}
//Установка формата вывода меню
bool init_menu_format(MENU* menu, int row, int column) { 
    if(row <= 0 || column <= 0)
        return false;

    menu->max_rows = row;
    menu->columns = column;
    menu->rows = menu->size / column + 1; 
    menu->items_abr = (ABREVIATED*)calloc(column, sizeof(ABREVIATED)); 
    for(int i = 0; i < column; i++) {
        menu->items_abr[i] = standart_abreviated;
    }
    if(menu->set_menu & STNDRT_COL_SIZE) {
        set_columns_size(menu, 0);
    }
    else if(menu->set_menu & NON_COL_SIZE) {
        calc_item_coord(menu);
    }
    return true;
}
//Установка размеров столбцов если они используются 
void set_columns_size(MENU *menu, double* col_len, ...) {
    if(col_len == 0) {
        menu->columns_size = (double*)calloc(menu->columns + 1, sizeof(double));
        for(int i = 0; i < menu->columns; i++) {
            menu->columns_size[i] = 1.0 / menu->columns;
        }
    } else {
        size_t num_columns = (size_t)col_len;
        va_list args;
        va_start(args, col_len);

        menu->columns_size = (double*)calloc(num_columns + 1, sizeof(double));
        for(int i = 0; i < num_columns; i++) {
            menu->columns_size[i] = va_arg(args, double);

            if(menu->columns_size[i] > 1.0) {
                if(getmaxx(menu->subwin) < 40) {
                    menu->max_column -= menu->columns_size[i] / 2.0;
                } else {
                    menu->max_column -= menu->columns_size[i];                 
                }
            }
        }

        va_end(args);
    }

    if(menu->set_menu & NON_COL_NAME) {
        calc_item_coord(menu);
    }

}
//Установка имен столбцов если используются 
void set_column_name(MENU *menu ,const wchar_t* names, ...) {
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
//Получение текущего размера столбца в пикселях 
int get_col_size(MENU* menu, int col, int item_num) {
    int size;
    if(menu->set_menu & NON_COL_SIZE) {
        return menu->items[item_num]->size;
    }
    else if(menu->columns_size[col] > 1.0) {
        if(getmaxx(menu->subwin) < 40 && menu->div_static_size) {
            size = menu->columns_size[col] / 2.0;
        } else {
            size = menu->columns_size[col];
        }
        return size;
         
    } else {
        size = menu->columns_size[col] * menu->max_column;
    }

    if(menu->set_menu & SPRT_INTERMEDIATE) {
        if(col == 0 || col == menu->columns - 1) {
            size -= 1;
        } else {
            size -= 2;      
        }
    }
    if((menu->set_menu & DESIG_ITEMS && (col == 0 && menu->type == F_HBOX))
                                        || (menu->type == GRID)) {
        size -= 1;
    }
    return size;
}
//Рассчет коориднатов для вывода элемета 
void calc_item_coord(MENU* menu) {
    if(menu->size == 0 || menu->items == NULL) {
        return;
    }

    size_t start_x; int start_y;
    int n_item;
    if(menu->select < menu->max_rows * menu->columns) {   
        start_y = n_item = 0;
    }
    else if(menu->size > menu->max_rows * menu->columns && menu->select >= menu->max_rows * menu->columns) {
        start_y = -1 - (menu->select - menu->max_rows * menu->columns) / menu->columns;
        n_item = 0;
    }

    bool is_break = false;
    while(!is_break) {
        start_x = 0;
        for(int j = 0; j < menu->columns; j++) {
            menu->items[n_item]->col = j;
            menu->items[n_item]->x = start_x;
            if(menu->set_menu & ALLIGMENT_CENTER) {
                menu->items[n_item]->x += (get_col_size(menu, j, n_item) / 2 - menu->items[n_item]->size / 2);
            } 

            if(menu->columns_size != NULL) {
                if(menu->columns_size[j] > 1.0) {
                    start_x += get_col_size(menu, j, n_item);
                } else {
                    start_x += menu->columns_size[j] * menu->max_column;
                }
            }

            if((menu->set_menu & SPRT_INTERMEDIATE && j != 0) 
                || (menu->set_menu & SPRT_ALL)) {
                menu->items[n_item]->x += 1;
            }

            if((menu->set_menu & DESIG_ITEMS && (menu->type == GRID)) 
                || ((menu->set_menu & DESIG_ITEMS && (menu->type == F_HBOX)) && (n_item % menu->columns == 0))) {
                menu->items[n_item]->x += 1;
            }

            if(menu->columns_size == NULL) {
                start_x = menu->items[n_item]->x + menu->items[n_item]->size;
            } 

            menu->items[n_item]->y = start_y; 
            if(menu->column_names != NULL) {
                menu->items[n_item]->y += 1;
            }
            n_item++;
            if(n_item == menu->size) {
                is_break = true;
                break;
            }
        }
        start_y++;
    } 
}
//Изменение размера меню, необходимо вызвать после изменения размера окна 
void resize_menu(MENU* menu) {
    menu->max_rows = getmaxy(menu->subwin);

    menu->max_column = getmaxx(menu->subwin);
    for(int i = 0; i < menu->columns && menu->set_menu & USER_COL_SIZE; i++) {
        if(menu->columns_size[i] > 1.0) {
            int col_size = get_col_size(menu, i, 0);
            menu->max_column -= col_size;
        }
    }

    if(menu->set_menu & USE_COL_NAME) {
        menu->max_rows -= 1;
    }

    calc_item_coord(menu);
} 

//-------------Вывод меню -------------
//Вывод имен столбцов 
void print_column_names(MENU *menu) {  
    int start_x = 0;
    int size_col = 0;
    int print_x = 0;

    for(int i = 0; i < menu->columns; i++) {
        if(menu->columns_size[i] > 1.0) {
            size_col = get_col_size(menu, i, 0);
            print_x = start_x + 1;
        } else {
            size_col = menu->columns_size[i] * menu->max_column;
            print_x = start_x + (size_col - menu->column_names[i]->size) / 2 + 1;
        }
        mvwaddwstr(menu->subwin, menu->column_names[i]->y, print_x, menu->column_names[i]->string);

        print_x += menu->column_names[i]->size - 1;

        if(i == menu->sorted_col && menu->is_sort) {
            if(menu->direction_sort == INCREASING) {
                mvwaddnwstr(menu->subwin, menu->column_names[i]->y, print_x, U_ARROW_UP, 1);
            } else if (menu->direction_sort == DECREASING) {
                mvwaddnwstr(menu->subwin, menu->column_names[i]->y, print_x, U_ARROW_DOWN, 1);
            }
        }

        start_x += size_col;
        if(menu->set_menu & SPRT_INTERMEDIATE && i != menu->columns - 1) {
            mvwaddnwstr(menu->subwin, 0, start_x, &menu->sprt_sym, 1);
        }
    }
}
//Вывод пункта меню по номеру 
void print_menu_point(MENU* menu, int num) {
        int offset = 1;
        short color;

        if((menu->set_menu & DESIG_ITEMS && menu->type == GRID) || num % menu->columns == 0) {
            if(menu->items[num]->pnt_sym == 0) {
                mvwaddch(menu->subwin, menu->items[num]->y, menu->items[num]->x - offset, menu->pnt_sym);
            } else {
                color = menu->items[num]->pnt_color == -1? menu->items[num]->color : menu->items[num]->pnt_color;
                mvwaddch(menu->subwin, menu->items[num]->y, menu->items[num]->x - offset, 
                        menu->items[num]->pnt_sym | COLOR_PAIR(color));
            }
            offset++;
        }

        if(menu->set_menu & SPRT_INTERMEDIATE) {
            color = menu->items[num]->pnt_color == -1? 0 : menu->items[num]->sprt_color;
            mvwaddnwstr_color(menu->subwin, menu->items[num]->y, menu->items[num]->x - offset--, &menu->sprt_sym , color, 1);
        } 

        bool is_free = false;  wchar_t* buffer;
        if(get_col_size(menu, menu->items[num]->col, num) < menu->items[num]->size) {
            buffer = menu->items_abr[menu->items[num]->col](menu->items[num]->string, get_col_size(menu, menu->items[num]->col, num));
            is_free = true;
        } else {
            buffer = menu->items[num]->string;
        }

        if(num == menu->select) {
            if(menu->set_menu & DESIG_ITEMS){
                mvwaddch(menu->subwin, menu->items[num]->y, menu->items[num]->x - offset, 
                    menu->slct_sym | COLOR_PAIR(color));
            }
            mvwaddwstr_color(menu->subwin, menu->items[num]->y , menu->items[num]->x, 
                buffer, menu->slctd_item_color_pair);
            
        } 
        else if(menu->group[num] != NULL) {
            if(menu->set_menu & DESIG_ITEMS){
                mvwaddch(menu->subwin, menu->items[num]->y, menu->items[num]->x - offset, 
                    menu->slct_sym | COLOR_PAIR(color));
            }
            mvwaddwstr_color(menu->subwin, menu->items[num]->y , menu->items[num]->x, 
                buffer, menu->group_color);
        }
        else {    
            color = menu->items[num]->color == -1? 0 : menu->items[num]->color;
            mvwaddwstr_color(menu->subwin, menu->items[num]->y , menu->items[num]->x, 
                    buffer, color);
        }
        if(menu->type == F_HBOX && num - (menu->columns - 1) == menu->select) {  
            recolor_str(menu->subwin, menu->iselect->y, menu->slctd_item_color_pair);
        } 

        if(is_free) {
            free(buffer);
        }
}
//Вывод меню 
void print_menu(MENU* menu) {
    if(menu == NULL) {
        return;
    }
    if(menu->set_menu & USE_COL_NAME) {
        print_column_names(menu);
    }

    int n_item;
    for(int i = 0; i < menu->size; i++) {
        if((menu->set_menu & NON_COL_NAME && menu->items[i]->y == 0 )
            || (menu->set_menu & USE_COL_NAME && menu->items[i]->y == 1)) {
            n_item = i;
            break;
        }
    }
    int item_in_page = menu->max_rows * menu->columns;
    for(int i = 0; i < item_in_page && i < menu->size; i++) {
        print_menu_point(menu, n_item);
        n_item++;
    }

}
//Очистка меню 
void unprint_menu(MENU* menu) {
    werase(menu->subwin);
}
//Для позиционирования по меню 
void menu_driver(MENU* menu, REQ_KEY key) {
    int prev_selected_item = menu->select;
    bool need_scroll = false;
    if(key & REQ_DOWN_ITEM) { 
        if((menu->select + menu->columns) < menu->size) { 
            menu->select += menu->columns; 
            need_scroll = true; 
        } 
    }
    if(key & REQ_UP_ITEM) { 
        if(menu->select - menu->columns >= 0) { 
            menu->select -= menu->columns; 
            need_scroll = true; 
        } 
    }
    if(key & REQ_LEFT_ITEM) { 
        if(menu->select > 0 && menu->type == GRID) { 
            menu->select--; 
            need_scroll = true; 
        } 
    }
    if(key & REQ_RIGHT_ITEM) { 
        if(menu->select < menu->size - 1 && menu->type == GRID) { 
            menu->select++; 
            need_scroll = true; 
        }  
    }
    if(need_scroll) {
        if(key & GROUP_ACT) {
            group_act(menu, prev_selected_item);
        }
        menu->iselect = menu->items[menu->select];
        change_menu_action(menu, key, prev_selected_item);
    }
}
//Прокрутка меню 
void scroll_menu(MENU* menu, REQ_KEY key) {
    scrollok(menu->subwin, true);
    switch (key)
    {
        case REQ_DOWN_ITEM: {
            wsetscrreg(menu->subwin, 1, menu->max_rows);
            
            offset_y_items(menu, -1);
            wscrl(menu->subwin, 1);
            break; 
        }
        case REQ_UP_ITEM: {
            offset_y_items(menu, 1);
            wscrl(menu->subwin, -1);
            break;
        }
        default: break;
    }
    scrollok(menu->subwin, false);
}
//Изменение выбранного элемента меню 
void change_menu_action(MENU* menu, REQ_KEY key, int prev_selected_item) {
    if(key & REQ_DOWN_ITEM) {
        if(menu->iselect->y == menu->max_rows + 1 && menu->set_menu & USE_COL_NAME) {
            scroll_menu(menu, REQ_DOWN_ITEM);
        }
        else if(menu->iselect->y == menu->max_rows && menu->set_menu & NON_COL_NAME) {
            scroll_menu(menu, REQ_DOWN_ITEM);
        }
    }
    if(key & REQ_UP_ITEM) {
        if(menu->iselect->y == -1 && menu->set_menu & NON_COL_NAME) {
            scroll_menu(menu, REQ_UP_ITEM);
        }
        else if(menu->iselect->y == 0 && menu->set_menu & USE_COL_NAME) {
            scroll_menu(menu, REQ_UP_ITEM);
        }
    }

    if(menu->type == GRID) {
        print_menu_point(menu, prev_selected_item);
        print_menu_point(menu, menu->select);
    } else if (menu->type == F_HBOX) {
        clear_y_str(menu->subwin, menu->items[prev_selected_item]->y, 0, 0);
        for(int i = 0; i < menu->columns; i++) {
            print_menu_point(menu, prev_selected_item + i);
            print_menu_point(menu, menu->select + i);
        }
    }
}
//Смещение координат меню вверх или вниз 
void offset_y_items(MENU* menu, int y) {
    int n_item = 0;
    bool is_break = false;
    while(!is_break) {
        for(int j = 0; j < menu->columns; j++) {
            menu->items[n_item++]->y += y; 
            if(n_item == menu->size) {
                is_break = true;
                break;
            }
        }
    } 
}
//Поиск нажатого элемента 
REQ_KEY find_click_item(MENU* menu, MEVENT event) {
    if(!wmouse_trafo(menu->subwin, &event.y, &event.x, FALSE)) {
        return NON_REQ;
    }

    if(event.y == 0 && menu->set_menu & USE_COL_NAME && event.bstate & BUTTON1_RELEASED) {
        size_t col_size = 0;
        for(int i = 0; i < menu->columns; i++) {
            if(event.x >= col_size && event.x <= col_size + get_col_size(menu, i, 0)) {
                if(i == menu->sorted_col) {
                    menu->direction_sort = menu->direction_sort == INCREASING? DECREASING : INCREASING;
                }
                menu->sorted_col = i;
                break;
            }
            col_size += get_col_size(menu, i, 0);
        }
        return COLUMN_CLICKED;
    }
    else if(event.bstate & BUTTON1_RELEASED){
        int prev_selected_item = menu->select;
        if(menu->type == F_HBOX) {
        for(int i = 0; i < menu->size; i += menu->columns) {
            if(event.y == menu->items[i]->y) {
                menu->select = i;
                break;
            }
        }
        }
        else if(menu->type == GRID) {
        for(int i = 0; i < menu->size; i++) {
            if(event.y == menu->items[i]->y && event.x >= menu->items[i]->x 
                && event.x < menu->items[i]->x + menu->items[i]->size) {
                menu->select = i;
                break;
            }
        }
        }
        if(prev_selected_item != menu->select) {
            menu->iselect = menu->items[menu->select];
            change_menu_action(menu, NON_REQ, prev_selected_item);
            return ITEM_CLICKED;
        } else if (prev_selected_item == menu->select) {
            return SAME_ITEM;
        }
    }
    return NON_REQ;
}
//Очистка элементов меню 
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
//Установка новых элементов в меню 
void set_new_items(MENU* menu, MITEM** new_items, int select) {
    free_items(menu->items);

    menu->size = size_items(new_items);
    menu->items = new_items;
    menu->select = select;
    menu->iselect = menu->items[menu->select];
    menu->rows = menu->size / menu->columns + 1;

    free(menu->group);
    menu->group = (MITEM**)calloc(menu->size + 1, sizeof(MITEM*));
    calc_item_coord(menu);
}
//Очистка меню 
void free_menu(MENU *menu) {
    free_items(menu->items);
    free(menu->columns_size);

    if(menu->column_names != NULL) {
        free_items(menu->column_names);
    }
    if(menu->columns_size != NULL) {
        //free(menu->columns_size);
    }

    free(menu);
}

wchar_t* standart_abreviated(wchar_t* wstr, int col_size) {
    int max_size = col_size;
    if(col_size <= 1) {
        max_size = 1;
    }

    wchar_t* abr_wsrt = (wchar_t*)calloc(max_size + 1, sizeof(wchar_t));
    abr_wsrt = wsubstring(wstr, wcslen(wstr) - max_size + 1, 0);
    abr_wsrt[0] = L'~';

    return abr_wsrt;
}