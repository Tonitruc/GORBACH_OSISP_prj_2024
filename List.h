#pragma once

#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>
#include "file_info_struct.h"

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
void sort_list(List* head, List* tail, int (*comparator)(FINFO*, FINFO*), int dir);
void free_list(List *head);

void print_list(List* head);
int sizeof_list(List* head);
List* get_tail(List* head);
