#include "memtrack_test.h"


//linked_list.c

typedef struct{

    Mem_Info *head;
    Mem_Info *tail;
    void (*fail_handler)(void*);
    void *handler_arg;
    bool memory_failure_abort;
    bool auto_null_pointers;
    bool init;
    pthread_mutex_t mutex;

} Track_Info;

static Track_Info info = {0};



int MemTrack_Init(void(*malloc_fail_handler)(void*), void *handler_arg, bool auto_null_pointers, bool memory_failure_abort){

    if(info.init){
        TRACK_PRINTF("Memtrack already initialized\n");
        return 1;
    }

    if(pthread_mutex_init(&info.mutex, NULL)) return 1;

    info.head = NULL;
    info.tail = NULL;

    info.auto_null_pointers = auto_null_pointers;
    info.memory_failure_abort = memory_failure_abort;

    info.fail_handler = malloc_fail_handler;
    info.handler_arg = handler_arg;

    return 0;
}




void free_tracking_info(){

    Mem_Info *current = info.head;
    Mem_Info *next = NULL;

    while(current){
        next = current->next;
        TRACK_FREE(current->file_name);
        TRACK_FREE(current);
        current = next;
    }

    info.head = NULL;
    info.tail = NULL;
}



size_t check_memory_usage(){

    pthread_mutex_lock(info.mutex);

    Mem_Info *current = info.head;
    size_t total = 0;

    while(current){
        total += current->size;
        current = current->next;
    }

    
    return total;
}


int check_memory_leak(){

    if(head || tail)
        return 1;

    return 0;
}



void print_tracking_info(){

    Mem_Info *current = head;

    if(!current)
        return;

    TRACK_PRINTF("\nAllocation Information\n");
    while(current){
        TRACK_PRINTF("size %lld - Line %d - File %s\n", current->size, current->file_line, current->file_name);
        current = current->next;
    }
    TRACK_PRINTF("\n"); 

}


int append_allocation(void *ptr, char *file, int line, size_t size){
    Mem_Info *node = TRACK_MALLOC(sizeof(Mem_Info));
    if(!node)
        return 1;
    node->size = size;
    node->ptr = ptr;
    node->file_name = TRACK_STRDUP(file);
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
    
    TRACK_FREE(current->file_name);
    TRACK_FREE(current);

    return 0;
}



//alloc.c





void check_malloc_error(void *mem){

    if(mem || !check_context_init())
        return;

    if(ctx->fail_handler)
        (*ctx->fail_handler)(ctx->handler_arg);

    TRACK_PRINTF("MemTrack ERROR: malloc failed\n");

    if(ctx->config.memory_failure_abort)
        TRACK_EXIT;
    
    return; 
}

void debug_check_malloc_error(void *mem, char *file, int line){

    if(mem || !check_context_init())
        return;

    if(ctx->fail_handler)
        (*ctx->fail_handler)(ctx->handler_arg);

    TRACK_PRINTF("MemTrack ERROR: malloc failed for file %s, line - %d\n", file, line);
    
    if(ctx->config.memory_failure_abort)
        TRACK_EXIT;
    
    return; 
}


char* safe_strdup(const char *src){
    char *new_mem = strdup(src);
    check_malloc_error(new_mem);
    return new_mem;
}

void* safe_realloc(void *memory, size_t size){
    void *new_mem = TRACK_REALLOC(memory, size);
    check_malloc_error(new_mem);
    return new_mem;
}

void* safe_malloc(size_t size){
    void *new_mem = TRACK_MALLOC(size);
    check_malloc_error(new_mem);
    return new_mem;
}


void safe_free(void **mem){

    if(!mem || !(*mem))
        return;

    TRACK_FREE(*mem);

    if(ctx->config.auto_null_pointers)
        *mem = NULL;

}

void debug_free(void **mem, char *file, int line){

    if(!mem || !(*mem))
        return;

    if(delete_allocation(*mem)){

        if(ctx->config.print_error_info)
            TRACK_PRINTF("MemTrack ERROR: failed to free old tracking info for file %s, line - %d\n", file, line);

        return;
    }
    TRACK_FREE(*mem);

    if(ctx->config.auto_null_pointers)
        *mem = NULL;
    
}


void* debug_malloc(size_t size, char *file, int line){

    void *mem = TRACK_MALLOC(size);

    debug_check_malloc_error(mem, file, line);
    if(!mem)
        return NULL;
    

    if(append_allocation(mem, file, line, size)){
        TRACK_FREE(mem);

        if(ctx->config.print_error_info)
            TRACK_PRINTF("MemTrack ERROR: failed to malloc tracking info for file %s, line - %d\n", file, line);

        return NULL;    
    }

    return mem;
}


void* debug_realloc(void *mem, size_t size, char *file, int line){

    if(!mem){
        void *new_mem = debug_malloc(size, file, line);
        return new_mem;
    }
        

    if(delete_allocation(mem)){

        if(ctx->config.print_error_info)
            TRACK_PRINTF("MemTrack ERROR: failed to free old tracking info for file %s, line - %d\n", file, line);

        return NULL;
    }
    
    void *new_mem = TRACK_REALLOC(mem, size);
    debug_check_malloc_error(new_mem, file, line);

    if(!new_mem)
        return NULL;
    

    if(append_allocation(new_mem, file, line, size)){
        TRACK_FREE(new_mem);

        if(ctx->config.print_error_info)
            TRACK_PRINTF("MemTrack ERROR: failed to malloc tracking info for file %s, line - %d\n", file, line);

        return NULL;            
    }


    return new_mem;
}

char* debug_strdup(const char* src, char *file, int line){

    if(!src)
        return NULL;

    size_t src_len = TRACK_STRLEN(src);

    char *dup = debug_malloc(sizeof(char) * (src_len + 1), file, line);
    if(!dup)
        return NULL;

    TRACK_STRCPY(dup, src);
    dup[src_len] = '\0';

    return dup;
}