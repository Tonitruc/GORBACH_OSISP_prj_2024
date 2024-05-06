#pragma once

#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>
#include "file_info_struct.h"


typedef struct _LIST_NODE {
    FINFO* data;
    struct _LIST_NODE *next;
    struct _LIST_NODE *prev;
} LIST_NODE;


typedef struct _LIST {
    LIST_NODE* head;
    LIST_NODE* tail;
    int size;
} LIST;

LIST* init_list();

void add_last(LIST* list, FINFO* data);
void add_first(LIST* list, FINFO* data);
void remove_n(LIST* list, int number);

void remove_last(LIST* list);
void remove_first(LIST* list);

void add_by(LIST* list, FINFO* data, int (*comparator)(const void* first, const void* second));
void sort_list(LIST_NODE* head, LIST_NODE* tail, int (*comparator)(FINFO*, FINFO*), int dir);
void free_list(LIST* list);
LIST_NODE* get_n_element(LIST* list, int num);

void print_list(LIST* list);
