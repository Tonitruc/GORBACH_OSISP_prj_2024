#pragma once

#include <time.h>
#include "file_type_enum.h"

typedef struct _FILE_INFO {
    wchar_t* file_name;
    wchar_t* full_path;
    long size_kb;
    time_t edit_time;
    FILE_TYPE file_type;
} FINFO;