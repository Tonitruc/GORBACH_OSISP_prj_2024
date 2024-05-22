#include "fwchar.h"

wchar_t* cstowchs(const char* char_string) { //Перевод обычной строки в расширенную
    size_t size_str = strlen(char_string) + 1;

    wchar_t* w_string = (wchar_t*)calloc(size_str, sizeof(wchar_t));
    mbstowcs(w_string, char_string, size_str);

    size_t size_wstr = wcslen(w_string) + 1;
    if(size_str != size_wstr) {
        w_string = (wchar_t*)realloc(w_string, size_wstr * sizeof(wchar_t));
    }

    return w_string;
}

wchar_t* inttowchs(int num) {  //Перевод числа в строку
    size_t num_size = 0;
    int temp = num;

    if(num < 0) {
        temp *= -1;
        num_size += 1;
    }
    while(temp != 0) {
        temp -= temp % 10;
        temp /= 10;
        num_size++;
    }

    wchar_t* wstr_num = (wchar_t*)calloc(num_size + 1, sizeof(wchar_t));
    swprintf(wstr_num, num_size + 1, L"%d", num);

    return wstr_num;
}

int wchstoint(wchar_t* num) {  //Перевод расширенной строки в число
    int int_value = wcstol(num, NULL, 10);

    return int_value;
}

char* wchtochs(wchar_t* wstr) {  //Перевод расширенной строки в обычную
    size_t w_size = wcslen(wstr) * sizeof(wchar_t) + 1;
    char* str = (char*)calloc(w_size + 1, sizeof(char)); 

    w_size = wcstombs(str, wstr, w_size + 1);

    return str;
}

wchar_t* wtime(time_t time) {  //Перевод времени в строку
    wchar_t* ws_time = (wchar_t*)calloc(STNDRT_TIME_SIZE + 1, sizeof(wchar_t));
    struct tm* s_time;

    s_time = localtime(&time);

    swprintf(ws_time, STNDRT_TIME_SIZE + 1, L"%d.%02d.%02d %02d:%02d", s_time->tm_year + 1900, s_time->tm_mon, s_time->tm_mday, 
                s_time->tm_hour, s_time->tm_min);

    return ws_time;
}

wchar_t* wsubstring(const wchar_t* wstring, int start, int len) { //Выделение подстроки из строки 
    size_t wstr_len = wcslen(wstring);
    if(start <= 0 || start > wstr_len || len < 0 || len + start > wstr_len + 2) {
        return NULL;
    }
    if(len == 0) {
        len = wstr_len - start + 1;
    }
    start--;
    wchar_t* wsubstr = (wchar_t*)calloc(len + 1, sizeof(wchar_t));
    for(int i = 0; i < len; i++) {
        wsubstr[i] = wstring[start++];
    }

    return wsubstr;
}

bool wchstrcmp(wchar_t* sourse, wchar_t* wstr, int start, int len) {  //Проверка вхождения в строку 
    size_t sourse_len = wcslen(sourse);
    size_t wstr_len = wcslen(wstr);
    if(start < 0 || start > sourse_len || len < 0 || len + start > sourse_len || len > wstr_len) {
        return false;
    }
    else if(len == 0 && wstr_len > sourse_len) {
        return false;
    }

    int i;
    if(len == 0 || start == 0) {
        i = wstr_len - 1;
    } else {
        i = len - 1;
    }

    if(start == 0) {
        start = sourse_len - 1;
    } else {
        if(len == 0) {
            start += i;
        } else {
            start += len - 1;
        }
    }

    for(; i >= 0; i--) {
        if(wstr[i] != sourse[start--]) {
            return false;
        }
    }

    return true;
}

wchar_t* parse_regex_pattern(wchar_t* wpattern) {  //Парс регулярного выражения для упрощенного ввода
    size_t size = wcslen(wpattern);
    wchar_t* pattern = (wchar_t*)calloc(size * 2 + 1, sizeof(wchar_t));
    int bi = 0;

    if(wpattern[0] == L'*') {
    // .*\.txt$
        pattern[bi++] = L'.'; 
        pattern[bi++] = L'*'; 
        pattern[bi++] = L'\\'; 

        for(int i = 1; i < size; i++) {
            pattern[bi++] = wpattern[i]; 
        }
        pattern[bi++] = '$';
    } 
    else if(wpattern[size - 1] == L'*') {
        pattern[bi++] = L'^'; 
        for(int i = 0; i < size; i++) {
            pattern[bi++] = wpattern[i]; 
        }
    }
    else {
        for(int i = 0; i < size; i++) {
            pattern[bi++] = wpattern[i]; 
        }
    }

    pattern[bi++] = '\0';
    return pattern;
}