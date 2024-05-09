#pragma once

#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <errno.h>
#include <regex.h>

#include "fwchar.h"
#include "List.h"
#include "file_type_enum.h"

#define UNIX_FILE_NAME_PATTERN "^[^/]*$"

wchar_t* get_cur_dir();
LIST* read_dir(char* path);
FILE_TYPE get_file_type(wchar_t* wfull_path);
FINFO* init_file_info(wchar_t* full_path, wchar_t* file_name, time_t edit_time, int size_kb, FILE_TYPE file_type);
int finfo_name_compare(FINFO* first, FINFO* second, int dir);
int finfo_size_compare(FINFO* first, FINFO* second, int dir);
int finfo_time_compare(FINFO* first, FINFO* second, int dir);
void rfind_files(LIST* result, char* start_dir, regex_t regex);
int find_files(LIST* result, wchar_t* wstart_dir, wchar_t* wpattern);
