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


wchar_t* cstowchs(const char* char_string);
wchar_t* inttowchs(int num);
wchar_t* wtime(time_t time);
wchar_t* get_wsmonth(time_t month);
int wget_month(wchar_t *month);
wchar_t* wtime(time_t time);
char* wchtochs(wchar_t* wstr);
wchar_t* wsubstring(const wchar_t* wstring, int start, int len);
bool wchstrcmp(wchar_t* sourse, wchar_t* wstr, int start, int len);
wchar_t* wcsscat(wchar_t* str1, wchar_t* str2);
int wcompare(const void* wstr1, const void* wstr2);
int wfcompare(const void* wstr1, const void* wstr2);
int wicompare(const void* wstr1, const void* wstr2);