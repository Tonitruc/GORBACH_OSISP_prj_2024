#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <wchar.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>

#define STNDRT_TIME_SIZE 13
#define SHORT_MONTH_SIZE 3

wchar_t* cstowchs(const char* char_string);
wchar_t* wchtoint(int num);
wchar_t* wtime(time_t time);
wchar_t* get_wsmonth(time_t month);
wchar_t* wtime(time_t time);
char* wchtochs(wchar_t* wstr);
wchar_t* wsubstring(const wchar_t* wstring, int start, int len);
bool wchstrcmp(wchar_t* sourse, wchar_t* wstr, int start, int len);
int wcompare(const void* wstr1, const void* wstr2);