#pragma once

#include "unistd.h"
#include <sys/types.h>

typedef struct List
{
    char* data;
    struct List *next;
    struct List *prev;

} List;

void add_last(List** head, List** tail, char*  data);
void add_first(List** head, List** tail, char*  data);
void remove_n(List** head, List** tail, int number);

void remove_last(List** head, List** tail);
void remove_first(List** head, List** tail);

void add_by(List **head, List **tail, char*  data, int (*comparator)(const void* first, const void* second));
void free_list(List *head);

void print_list(List* head);
int sizeof_list(List* head);
