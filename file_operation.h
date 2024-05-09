#pragma once

#include <fcntl.h>

#include "file_type_enum.h"
#include "file_types.h"
#include "fwchar.h"

typedef enum _FILE_OPERATION_RESULT {
    SDIR_NOT_EXIST = 3,
    RDIR_NOT_EXITS = 2,
    OPERATION_ERROR = 0,
    SUCCESS = 1,
    NAME_EXIST = 4,
    EMPTY_NAME = 5,
} FOPR;

FOPR delete_file(wchar_t* full_path, bool is_dir);
FOPR create_directory(wchar_t* full_path);
FOPR create_file(wchar_t* full_path);
FOPR move_file(wchar_t* file_path, wchar_t* new_dir);
int file_name_validator(int ch);
FOPR cpyfile(wchar_t* file_path, wchar_t* new_dir, int save_atr, int link);
FOPR cpydir(wchar_t* copy_dir, wchar_t* new_dir, int save_atr, int link);
FOPR create_slnk(wchar_t* dir, wchar_t* name);
wchar_t* find_slnk_path(wchar_t* slnk);
FOPR rnm_file(wchar_t* path, wchar_t* new_name);
FOPR cpyslnk(wchar_t* copy_link, wchar_t* new_dir, int save_attr);