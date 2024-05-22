#pragma once

//____________ Библиотека для работы с расширенными символами (wchat_t)________________

#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <wchar.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>

#define STNDRT_TIME_SIZE 16
#define SHORT_MONTH_SIZE 3

//Конвертация расширенных и обычных символов
wchar_t* cstowchs(const char* char_string);
wchar_t* inttowchs(int num);
char* wchtochs(wchar_t* wstr);

//Конвертация времени в строку
wchar_t* wtime(time_t time);

//Дополнительные функции 
wchar_t* wsubstring(const wchar_t* wstring, int start, int len);
bool wchstrcmp(wchar_t* sourse, wchar_t* wstr, int start, int len);
wchar_t* parse_regex_pattern(wchar_t* wpattern);