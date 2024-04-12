#pragma once

#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>
#include "file_type_enum.h"

typedef struct _FILE_INFO {
    wchar_t* file_name;
    wchar_t* full_path;
    long size_kb;
    time_t edit_time;
    FILE_TYPE file_type;
} FINFO;

typedef struct List
{
    FINFO* data;
    struct List *next;
    struct List *prev;

} List;

void add_last(List** head, List** tail, FINFO* data);
void add_first(List** head, List** tail, FINFO* data);
void remove_n(List** head, List** tail, int number);

void remove_last(List** head, List** tail);
void remove_first(List** head, List** tail);

void add_by(List **head, List **tail, FINFO* data, int (*comparator)(const void* first, const void* second));
void free_list(List *head);

void print_list(List* head);
int sizeof_list(List* head);
