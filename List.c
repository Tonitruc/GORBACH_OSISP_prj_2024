#include "List.h"

LIST* init_list() {
    LIST* list = (LIST*)calloc(1, sizeof(LIST));
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    return list;
}

void add_last(LIST* list, FINFO* data) {
    LIST_NODE* new_node = (LIST_NODE*)malloc(sizeof(LIST_NODE));
    new_node->next = NULL;
    new_node->prev = NULL;
    new_node->data = data;

    if(list->head == NULL) {
        list->head = new_node;
        list->tail = list->head;
    }
    else if(list->head == list->tail) {
        list->tail = new_node;
        (list->tail)->prev = list->head;
        (list->head)->next = list->tail;
    }
    else {
        new_node->prev = list->tail;
        (list->tail)->next = new_node;
        (list->tail) = new_node;
    }
    list->size++;
}

void add_first(LIST* list, FINFO* data) {
    LIST_NODE* new_node = (LIST_NODE*)malloc(sizeof(LIST_NODE));
    new_node->next = NULL;
    new_node->prev = NULL;
    new_node->data = data;

    if(list->head == NULL) {
        list->head = new_node;
        list->tail = list->head;
    }
    else if(list->head == list->tail) {
        list->head->prev = new_node;
        new_node->next = list->head;
        list->head = new_node;
    }
    else {
        new_node->next = list->head;
        list->head->prev = new_node;
        list->head = new_node;
    }
    list->size++;
}

void remove_n(LIST* list, int number) {
    if(number < 1)
        return;
    LIST_NODE* delete_node = list->head;

    if(number == 1 && list->head == list->tail) 
        list->head = list->tail = NULL;
    else {
        while(number - 1 != 0) {
            number--;
            delete_node = delete_node->next;
        }

        if(delete_node->prev != NULL)
            delete_node->prev->next = delete_node->next;
        else
            delete_node->next->prev = NULL;

        if(delete_node->next != NULL)
            delete_node->next->prev = delete_node->prev;
        else 
            delete_node->prev->next = NULL;
    }

    free(delete_node);
    list->size--;
}

void remove_last(LIST* list) {
    if(list->head == NULL)
        return;

    LIST_NODE *delete_el = list->tail;
    if(list->head == list->tail) {
        list->head = list->tail = NULL;
    }
    else {
        list->tail = list->tail->prev;
        list->tail->next = NULL;
    }

    free(delete_el);
    list->size--;
}

void remove_first(LIST* list) {
    if(list->head == NULL)
        return;

    LIST_NODE *delete_el = list->head;
    if(list->head == list->tail) {
        list->head = list->tail = NULL;
    }
    else {
        list->head = list->head->next;
        list->head->prev = NULL;
    }

    free(delete_el);
    list->size--;
}

void swap_finfo(FINFO** first, FINFO** second) {
    FINFO* temp = *first;
    *first = *second;
    *second = temp;
}

LIST_NODE* division(LIST_NODE* head, LIST_NODE* tail, int (*comparator)(FINFO*, FINFO*, int), int dir) {
    FINFO* data = tail->data;
    LIST_NODE* ave = head->prev;

    for(LIST_NODE* left = head; left != tail; left = left->next) {
        if(comparator(left->data, data, dir) <= 0 && dir == 0) {
            ave = ave == NULL? head : ave->next;
            swap_finfo(&(ave->data), &(left->data));
        } else if(comparator(left->data, data, dir) >= 0 && dir == 1) {
            ave = ave == NULL? head : ave->next;
            swap_finfo(&(ave->data), &(left->data));
        } 
    }
    ave = ave == NULL? head : ave->next;
    swap_finfo(&(ave->data), &(tail->data));
    return ave;
}

void sort_list(LIST_NODE* head, LIST_NODE* tail, int (*comparator)(FINFO*, FINFO*, int), int dir) {
    if (tail != NULL && head != tail && head != tail->next) 
    { 
        LIST_NODE* div = division(head, tail, comparator, dir); 
        sort_list(head, div->prev, comparator, dir); 
        sort_list(div->next, tail, comparator, dir); 
    } 
}

void free_list(LIST* list) {
    if(list->head == NULL)
        return;

    LIST_NODE *prev ;
    while(list->head != NULL) {
        prev = list->head;
        list->head = list->head->next;
        free(prev);
    }
}

LIST_NODE* get_n_element(LIST* list, int num) {
    if(num < 0 || num >= list->size) {
        return NULL;
    }

    LIST_NODE* temp = list->head;
    while(num != 0) {
        temp = temp->next;
        num--;
    }

    return temp;
}
