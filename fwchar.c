#include "fwchar.h"

wchar_t* get_wsmonth(time_t month) {
    struct tm* tm_month;
    tm_month = gmtime(&month);

    wchar_t* ws_month = (wchar_t*)calloc(SHORT_MONTH_SIZE + 1, sizeof(wchar_t));

    switch (tm_month->tm_mon)
    {
    case 1: {wcscpy(ws_month, L"янв"); break;}
    case 2: {wcscpy(ws_month, L"фев"); break;}
    case 3: {wcscpy(ws_month, L"мар"); break;}
    case 4: {wcscpy(ws_month, L"апр"); break;}
    case 5: {wcscpy(ws_month, L"май"); break;}
    case 6: {wcscpy(ws_month, L"июн"); break;}
    case 7: {wcscpy(ws_month, L"июл"); break;}
    case 8: {wcscpy(ws_month, L"авг"); break;}
    case 9: {wcscpy(ws_month, L"сен"); break;}
    case 10: {wcscpy(ws_month, L"окт"); break;}
    case 11: {wcscpy(ws_month, L"ноя"); break;}
    case 12: {wcscpy(ws_month, L"дек"); break;}
    }

    return ws_month;
}

wchar_t* cstowchs(const char* char_string) {
    size_t size_str = strlen(char_string) + 1;

    wchar_t* w_string = (wchar_t*)calloc(size_str, sizeof(wchar_t));
    mbstowcs(w_string, char_string, size_str);

    size_t size_wstr = wcslen(w_string) + 1;
    if(size_str != size_wstr) {
        w_string = (wchar_t*)realloc(w_string, size_wstr * sizeof(wchar_t));
    }

    return w_string;
}

wchar_t* wchtoint(int num) {
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

char* wchtochs(wchar_t* wstr) {
    size_t w_size = wcslen(wstr) * sizeof(wchar_t) + 1;
    char* str = (char*)calloc(w_size + 1, sizeof(char)); 

    w_size = wcstombs(str, wstr, w_size + 1);

    return str;
}

wchar_t* wtime(time_t time) {
    wchar_t* ws_time = (wchar_t*)calloc(STNDRT_TIME_SIZE + 1, sizeof(wchar_t));
    struct tm* s_time;

    s_time = gmtime(&time);
    wchar_t* ws_month = get_wsmonth(time);

    swprintf(ws_time, STNDRT_TIME_SIZE + 1, L"%ls %2d %02d:%02d", ws_month, s_time->tm_mday, s_time->tm_hour, s_time->tm_min);
    free(ws_month);

    return ws_time;
}

wchar_t* wsubstring(const wchar_t* wstring, int start, int len) {
    size_t wstr_len = wcslen(wstring);
    if(start <= 0 || start > wstr_len || len < 0 || len + start > wstr_len) {
        return NULL;
    }

    wchar_t* wsubstr = (wchar_t*)calloc(len + 1, sizeof(wchar_t));
    for(int i = 0; i < len; i++) {
        wsubstr[i] = wstring[--start];
    }

    return wsubstr;
}

bool wchstrcmp(wchar_t* sourse, wchar_t* wstr, int start, int len) {
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

    for(i; i >= 0; i--) {
        if(wstr[i] != sourse[start--]) {
            return false;
        }
    }

    return true;
}

int wcompare(const void* wstr1, const void* wstr2) {
    return wcscmp((const wchar_t*)wstr1, (const wchar_t*)wstr2);
}