#pragma once

#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>
#include "fwchar.h"
#include "List.h"
#include <stdbool.h>
#include "file_type_enum.h"

 List* read_dir(char* path);
 FILE_TYPE get_file_type(wchar_t* wfull_path);
 FINFO* init_file_info(wchar_t* full_path, wchar_t* file_name, time_t edit_time, int size_kb, FILE_TYPE file_type);
