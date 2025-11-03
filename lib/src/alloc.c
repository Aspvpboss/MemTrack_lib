#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "MemTrack_linked_list.h"
#include "MemTrack_export.h"


static MemTrack_Context *ctx = NULL;

int check_context_init(){

    if(!ctx){
        fprintf(stderr, "MemTrack ERROR: Set_MemTrack_Context hasn't been called yet\naborting program\n");
        abort();
        return 0;
    }
        

    return 1;
}


MemTrack_API void Set_MemTrack_Context(MemTrack_Context *e_ctx){

    ctx = e_ctx;
    ctx->config.auto_null_pointers = false;
    ctx->config.memory_failure_abort = false;
    ctx->config.print_error_info = false;

}


MemTrack_API void Set_Malloc_Error_Function(void(*function)(void*), void *function_arg){

    if(!check_context_init())
        return;
    ctx->fail_handler = function;
    ctx->handler_arg = function_arg;

    return;
}





void check_malloc_error(void *mem){

    if(mem || !check_context_init())
        return;

    if(ctx->fail_handler)
        (*ctx->fail_handler)(ctx->handler_arg);

    
    if(ctx->config.print_error_info)
        fprintf(stderr, "MemTrack ERROR: malloc failed\n");

    if(ctx->config.memory_failure_abort)
        abort();
    
    return; 
}

void debug_check_malloc_error(void *mem, char *file, int line){

    if(mem || !check_context_init())
        return;

    if(ctx->fail_handler)
        (*ctx->fail_handler)(ctx->handler_arg);

    if(ctx->config.print_error_info)
        fprintf(stderr, "MemTrack ERROR: malloc failed for file %s, line - %d\n", file, line);
    
    if(ctx->config.memory_failure_abort)
        abort();
    
    return; 
}


MemTrack_API char* safe_strdup(const char *src){
    char *new_mem = strdup(src);
    check_malloc_error(new_mem);
    return new_mem;
}

MemTrack_API void* safe_realloc(void *memory, size_t size){
    void *new_mem = realloc(memory, size);
    check_malloc_error(new_mem);
    return new_mem;
}

MemTrack_API void* safe_malloc(size_t size){
    void *new_mem = malloc(size);
    check_malloc_error(new_mem);
    return new_mem;
}


MemTrack_API void safe_free(void **mem){

    if(!mem || !(*mem))
        return;

    free(*mem);

    if(ctx->config.auto_null_pointers)
        *mem = NULL;

}

MemTrack_API void debug_free(void **mem, char *file, int line){

    if(!mem || !(*mem))
        return;

    if(delete_allocation(*mem)){

        if(ctx->config.print_error_info)
            fprintf(stderr, "MemTrack ERROR: failed to free old tracking info for file %s, line - %d\n", file, line);

        return;
    }
    free(*mem);

    if(ctx->config.auto_null_pointers)
        *mem = NULL;
    
}


MemTrack_API void* debug_malloc(size_t size, char *file, int line){

    void *mem = malloc(size);

    debug_check_malloc_error(mem, file, line);
    if(!mem)
        return NULL;
    

    if(append_allocation(mem, file, line, size)){
        free(mem);

        if(ctx->config.print_error_info)
            fprintf(stderr, "MemTrack ERROR: failed to malloc tracking info for file %s, line - %d\n", file, line);

        return NULL;    
    }

    return mem;
}


MemTrack_API void* debug_realloc(void *mem, size_t size, char *file, int line){

    if(!mem){
        void *new_mem = debug_malloc(size, file, line);
        return new_mem;
    }
        

    if(delete_allocation(mem)){

        if(ctx->config.print_error_info)
            fprintf(stderr, "MemTrack ERROR: failed to free old tracking info for file %s, line - %d\n", file, line);

        return NULL;
    }
    
    void *new_mem = realloc(mem, size);
    debug_check_malloc_error(new_mem, file, line);

    if(!new_mem)
        return NULL;
    

    if(append_allocation(new_mem, file, line, size)){
        free(new_mem);

        if(ctx->config.print_error_info)
            fprintf(stderr, "MemTrack ERROR: failed to malloc tracking info for file %s, line - %d\n", file, line);

        return NULL;            
    }


    return new_mem;
}

MemTrack_API char* debug_strdup(const char* src, char *file, int line){

    if(!src)
        return NULL;

    size_t src_len = strlen(src);

    char *dup = debug_malloc(sizeof(char) * (src_len + 1), file, line);
    if(!dup)
        return NULL;

    strcpy(dup, src);
    dup[src_len] = '\0';

    return dup;
}