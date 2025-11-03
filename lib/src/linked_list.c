#include "MemTrack_linked_list.h"
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static Mem_Info *head = NULL;
static Mem_Info *tail = NULL;


MemTrack_API void free_tracking_info(){
    Mem_Info *current = head;
    Mem_Info *next = NULL;

    while(current){
        next = current->next;
        free(current->file_name);
        free(current);
        current = next;
    }

    head = NULL;
    tail = NULL;
}



MemTrack_API size_t check_memory_usage(){
    Mem_Info *current = head;
    size_t total = 0;

    while(current){
        total += current->size;
        current = current->next;
    }

    return total;
}


MemTrack_API int check_memory_leak(){

    if(head || tail)
        return 1;

    return 0;
}



MemTrack_API void print_tracking_info(){

    Mem_Info *current = head;

    if(!current)
        return;

    printf("\nAllocation Information\n");
    while(current){
        printf("Address %p - size %lld - Line %d - File %s\n", current->ptr, current->size, current->file_line, current->file_name);
        current = current->next;
    }
    printf("\n");

}


int append_allocation(void *ptr, char *file, int line, size_t size){
    Mem_Info *node = malloc(sizeof(Mem_Info));
    if(!node)
        return 1;
    node->size = size;
    node->ptr = ptr;
    node->file_name = strdup(file);
    node->file_line = line;

    if(tail){

        tail->next = node;
        tail = node;
        tail->next = NULL;
        return 0;
    }

    if(!head){

        head = node;
        head->next = NULL;
        tail = head;
        return 0;
    }

    return 1;
}


int delete_allocation(void *check_ptr){
    Mem_Info *current = head;
    Mem_Info *prev = NULL;

    while(current && current->ptr != check_ptr){

        prev = current;
        current = current->next;

    }

    if(!current){
        return 1;
    }

    if(current->next && prev){ // not head, not tail

        prev->next = current->next;
    
    } else if(current->next && !prev){ // head, not tail

        head = current->next;

    } else if(!current->next && prev){ // not head, tail

        prev->next = NULL;
        tail = prev;

    } else{ // one node

        head = NULL;
        tail = NULL;

    }
    
    free(current->file_name);
    free(current);

    return 0;
}