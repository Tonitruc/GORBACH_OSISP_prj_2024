#include "text_box.h"
//Инициализация формы (род. окно, высота, ширина, расположение, название, сообщение, регулярное выражение, подтверждение ввода)
TEXT_BOX* init_text_box(WINDOW* par_win, int height, int width, int y, int x, wchar_t* title, wchar_t* message, const char* pattern, bool verify) {
    int start_x = x, start_y = y;
    if(x == 0 && y == 0) { //По центру экрнаа 
        CENTER_SCR(start_y, start_x);
        start_y -= height / 2;
        start_x -= width / 2;
    }

    TEXT_BOX* text_box = (TEXT_BOX*)calloc(1, sizeof(TEXT_BOX));
    text_box->start_x = start_x;
    text_box->start_y = start_y;
    text_box->window = derwin(par_win, height, width, start_y, start_x);

    text_box->input = (wchar_t*)calloc(1, sizeof(wchar_t));
    text_box->input[0] = '\0';

    text_box->height = height;
    text_box->width = width;
    text_box->pattern = pattern;
    text_box->box = true;
    text_box->verify = verify;
    
    text_box->title = (wchar_t*)calloc(wcslen(title + 1), sizeof(wchar_t*));
    text_box->message = (wchar_t*)calloc(wcslen(message + 1), sizeof(wchar_t*));
    wattron(text_box->window, COLOR_PAIR(TOP_PANEL_COLOR));
    wbkgd(text_box->window, COLOR_PAIR(TOP_PANEL_COLOR));
    werase(text_box->window);
    keypad(text_box->window, TRUE);
    wcscpy(text_box->title, title);
    wcscpy(text_box->message, message);
    //Создание поля для ввода максимальный размеро 255 символов 
    FIELD **field = (FIELD**)calloc(2, sizeof(FIELD*));
    field[0] = new_field(1, width - 5, 0, 0, 0, 0);

    text_box->fields = field;

    text_box->form = new_form(field);
    set_form_win(text_box->form, text_box->window);
    WINDOW* form_win = derwin(text_box->window, 1, width - 4, text_box->height / 2, 2);
    keypad(form_win, TRUE);

    set_form_sub(text_box->form, form_win);
    set_field_opts(field[0], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);
    if(pattern != NULL) {
        set_field_type(field[0], TYPE_REGEXP, pattern);
    }

    text_box->menu = NULL;

    if(text_box->verify) {
        MENU* menu = verify_menu(text_box->window);
        text_box->menu = menu;
    }

    return text_box;
}
//Установка цвета формы 
void set_color_text(TEXT_BOX* tb, short color_pair) {
    tb->color_pair = color_pair;
    wattron(tb->window, COLOR_PAIR(color_pair));
    wbkgd(tb->window, COLOR_PAIR(color_pair));
}
//Вывод формы 
void show_text_box(TEXT_BOX* tb) {
    wrefresh(tb->form->sub);
    wrefresh(tb->window);
    post_form(tb->form);

    mvwaddwstr(tb->window, tb->height / 2 - 1, 2, tb->message);
    if(tb->box) {
        box(tb->window, 0, 0);
    }
    mvwaddwstr(tb->window, 0, (tb->width - 2) / 2 - wcslen(tb->title) / 2, tb->title);

    if(tb->verify) {
        print_menu(tb->menu);
    }
}
//Получение размера введенного сообщения 
size_t get_field_len(FIELD* field) {
    char* buffer = field_buffer(field, 0);
    size_t len = strlen(buffer);
    int i = len - 1;
    while(buffer[i] == ' ' && i > 0) i--;
    len = i + 1;
    return len;
}
//Установка стартового текста в форме 
void set_start_text(TEXT_BOX* tb, wchar_t* text) {
    size_t size = wcslen(text);
    for(int i = 0; i < size; i++) {
        form_driver_w(tb->form, 0, text[i]);
    }
}
//Получение данных после ввода 
wchar_t* get_field_buffer(FORM* form) {
    if(form_driver(form, REQ_VALIDATION) != E_OK) {
        return NULL;
    }
    int len = get_field_len(form->field[0]); 
    mvprintw(1, 0, "%d", len);
    char* buffer = (char*)calloc(len + 1, sizeof(char));
    char* inp_str = field_buffer(form->field[0], 0);
    strncpy(buffer, inp_str, len);
    buffer[len] = '\0';
    if(strlen(inp_str) == 0) {
        return NULL;
    }    
    wchar_t* result = cstowchs(buffer);
    free(buffer);
    return result;
}
//Сохранение данных ввода и получения их 
wchar_t* save_input(TEXT_BOX* tb) {
    free(tb->input);
    tb->input = get_field_buffer(tb->form);
    if(tb->input == NULL) {
        return NULL;
    }

    wchar_t* copy = (wchar_t*)calloc(wcslen(tb->input) + 1, sizeof(wchar_t));
    wcscpy(copy, tb->input);

    return copy;
}
//Ввод 
TEXT_REQ input_text_box(TEXT_BOX* tb) {
    show_text_box(tb);
    int status = -2;
    int key; wint_t sym;

    do {
        curs_set(true); //Установка курсора в конец 
        form_driver(tb->form, ' ');
        form_driver(tb->form, REQ_DEL_PREV);
        
        if(tb->verify) {
            wnoutrefresh(tb->menu->subwin);
            print_menu(tb->menu);
        }
        wnoutrefresh(tb->window);
        doupdate();
        key = wget_wch(tb->window, &sym); //Если нажат ENTER 
        if((sym == '\n' || sym == KEY_ENTER) && tb->verify) {
            if(tb->menu->select == 0) {                          
                status = T_ALLOW;
            } else {
                status = T_CANCEL;
            }
            break;
        } 
        else if (sym == KEY_RIGHT) { //Обработка кнопок влево и вправо 
            form_driver(tb->form, REQ_NEXT_CHAR);
            if(tb->verify) {
                menu_driver(tb->menu, REQ_RIGHT_ITEM);
            }
        } 
        else if (sym == KEY_LEFT && tb->verify) {
            form_driver(tb->form, REQ_PREV_CHAR);
            if(tb->verify) {
                menu_driver(tb->menu, REQ_LEFT_ITEM);
            }
        }
        else if(sym == KEY_MOUSE) { // Обработка нажатий мыши
            MEVENT mevent;
            if(getmouse(&mevent) == OK && mevent.bstate &  BUTTON1_RELEASED) {
                if(wenclose(tb->window, mevent.y, mevent.x) && tb->verify) {
                    find_click_item(tb->menu, mevent);
                } else if(!wenclose(tb->window, mevent.y, mevent.x))  {
                    curs_set(false);
                    return T_CANCEL;
                }
            }
        } 
        else if(sym == KEY_BACKSPACE) { //Обработка клавиши backspace 
            form_driver(tb->form, REQ_DEL_PREV);
        } 
        else if(sym == KEY_RESIZE) {
            status = T_CANCEL;
        }
        else {
            wchar_t wsym = (wchar_t)sym;
            form_driver_w(tb->form, key, wsym);
        }

    } while(status == -2);
    curs_set(false);

    wrefresh(tb->window);
    return status;
}
//Очистка формы 
void free_text(TEXT_BOX* tb) {
    if(tb->verify) {
        free_menu(tb->menu);
    }

    free(tb->input);
    unpost_form(tb->form);
    free_form(tb->form);
    free(tb->title);
    free(tb->message);

    delwin(tb->window);
    free(tb);
}