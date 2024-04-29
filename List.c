#include "List.h"

void add_last(List** head, List** tail, FINFO* data) {
    List* new_node = (List*)malloc(sizeof(List));
    new_node->next = NULL;
    new_node->prev = NULL;
    new_node->data = data;

    if(*head == NULL) {
        *head = new_node;
        *tail = *head;
    }
    else if(*head == *tail) {
        *tail = new_node;
        (*tail)->prev = *head;
        (*head)->next = *tail;
    }
    else {
        new_node->prev = *tail;
        (*tail)->next = new_node;
        (*tail) = new_node;
    }
}

void add_first(List** head, List** tail, FINFO* data) {
    List* new_node = (List*)malloc(sizeof(List));
    new_node->next = NULL;
    new_node->prev = NULL;
    new_node->data = data;

    if(*head == NULL) {
        *head = new_node;
        *tail = *head;
    }
    else if(*head == *tail) {
        (*head)->prev = new_node;
        new_node->next = *head;
        *head = new_node;
    }
    else {
        new_node->next = *head;
        (*head)->prev = new_node;
        (*head) = new_node;
    }
}

void remove_n(List** head, List** tail, int number) {
    if(number < 1)
        return;
    List* delete_node = *head;

    if(number == 1 && *head == *tail) 
        *head = *tail = NULL;
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
}

void remove_last(List** head, List** tail) {
    if(head == NULL)
        return;

    List *delete_el = *tail;
    if(*head == *tail) {
        *head = *tail = NULL;
    }
    else {
        (*tail) = (*tail)->prev;
        (*tail)->next = NULL;
    }

    free(delete_el);
}

void remove_first(List **head, List **tail) {
    if(head == NULL)
        return;

    List *delete_el = *head;
    if(*head == *tail) {
        *head = *tail = NULL;
    }
    else {
        (*head) = (*head)->next;
        (*head)->prev = NULL;
    }

    free(delete_el);
}

void add_by(List **head, List **tail, FINFO* data, int (*comparator)(const void* first, const void* second)) {
    List* new_node = (List*)malloc(sizeof(List));
    new_node->next = NULL;
    new_node->prev = NULL;
    new_node->data = data;

    if(*head == NULL) {
        *head = new_node;
        *tail = *head;
    }
    else if(*tail == *head) {
        if(comparator(&data, &(*head)->data) >= 0) {
            *tail = new_node;
            (*tail)->prev = *head;
            (*head)->next = *tail;
        }
        else {
            (*head)->prev = new_node;
            new_node->next = *head;
            *head = new_node;
        }
    }
    else {
        List* temp = *head;
        while(comparator(&data, &temp->data) >= 0 && temp->next != NULL)
            temp = temp->next;

        new_node->prev = temp->prev;
        if(temp->prev != NULL)
            new_node->prev->next = new_node;

        new_node->next = temp;
        temp->prev = new_node;
    }
}

void swap_finfo(FINFO** first, FINFO** second) {
    FINFO* temp = *first;
    *first = *second;
    *second = temp;
}

List* division(List* head, List* tail, int (*comparator)(FINFO*, FINFO*), int dir) {
    FINFO* data = tail->data;
    List* ave = head->prev;

    for(List* left = head; left != tail; left = left->next) {
        if(comparator(left->data, data) <= 0 && dir == 1) {
            ave = ave == NULL? head : ave->next;
            swap_finfo(&(ave->data), &(left->data));
        } 
        else if(comparator(left->data, data) >= 0 && dir == 0) {
            ave = ave == NULL? head : ave->next;
            swap_finfo(&(ave->data), &(left->data));
        } 
    }
    ave = ave == NULL? head : ave->next;
    swap_finfo(&(ave->data), &(tail->data));
    return ave;
}

void sort_list(List* head, List* tail, int (*comparator)(FINFO*, FINFO*), int dir) {
    if (tail != NULL && head != tail && head != tail->next) 
    { 
        List* div = division(head, tail, comparator, dir); 
        sort_list(head, div->prev, comparator, dir); 
        sort_list(div->next, tail, comparator, dir); 
    } 
}

int sizeof_list(List* head) {
    int num = 0;
    while(head != NULL) {
        num++; 
        head = head->next;
    }
    return num;
}

void free_list(List *head) {
    if(head == NULL)
        return;

    List *prev ;
    while(head != NULL) {
        prev = head;
        head = head->next;
        free(prev);
    }
}

List* get_tail(List* head) {
    if(head == NULL) {
        return NULL;
    }

    while(head->next != NULL) {
        head = head->next;
    }

    return head;
}