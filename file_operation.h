#pragma once

#include <fcntl.h>

#include "file_type_enum.h"
#include "file_types.h"
#include "fwchar.h"

int delete_file(wchar_t* full_path);
bool create_directory(wchar_t* full_path);
bool create_file(wchar_t* full_path);
bool move_file(wchar_t* file_path, wchar_t* new_dir);
int file_name_validator(int ch);
bool cpyfile(wchar_t* file_path, wchar_t* new_dir);
bool cpydir(wchar_t* copy_dir, wchar_t* new_dir);