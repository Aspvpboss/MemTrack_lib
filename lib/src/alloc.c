#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "MemTrack_linked_list.h"
#include "MemTrack_export.h"

static void(*fail_handler)(void*) = NULL;
static void *handler_arg = NULL;

MemTrack_API void Set_Malloc_Error_Function(void(*function)(void*), void *function_arg){

    fail_handler = function;
    handler_arg = function_arg;

}


MemTrack_API void check_malloc_error(void *mem){

    if(mem)
        return;

    if(fail_handler)
        (*fail_handler)(handler_arg);

    fprintf(stderr, "MemTrack ERROR: malloc failed\n");

    #ifdef MEMORY_FAILIURE_ABORT
        abort();
    #endif
    
    return; 
}

MemTrack_API void debug_check_malloc_error(void *mem, char *file, int line){

    if(mem)
        return;

    if(fail_handler)
        (*fail_handler)(handler_arg);

    fprintf(stderr, "MemTrack ERROR: malloc failed for file %s, line - %d\n", file, line);

    #ifdef MEMORY_FAILIURE_ABORT
        abort();
    #endif
    
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
    *mem = NULL;

}

MemTrack_API void debug_free(void **mem, char *file, int line){

    if(!file){
        printf("MemTrack ERROR: debug_free doesn't accept 'NULL' into 'char *file'\n");
        return;
    }

    if(!mem || !(*mem))
        return;

    if(delete_allocation(*mem)){
        printf("MemTrack ERROR: failed to free old tracking info for file %s, line - %d\n", file, line);
        return;
    }
    free(*mem);
    *mem = NULL;
    
}


MemTrack_API void* debug_malloc(size_t size, char *file, int line){

    if(!file){
        printf("MemTrack ERROR: debug_free doesn't accept 'NULL' into 'char *file'\n");
        return NULL;
    }

    void *mem = malloc(size);
    debug_check_malloc_error(mem, file, line);
    if(!mem)
        return NULL;
    

    if(append_allocation(mem, file, line, size)){
        free(mem);
        printf("MemTrack ERROR: failed to malloc tracking info for file %s, line - %d\n", file, line);
        return NULL;    
    }

    return mem;
}


MemTrack_API void* debug_realloc(void *mem, size_t size, char *file, int line){

    if(!file){
        printf("MemTrack ERROR: debug_free doesn't accept 'NULL' into 'char *file'\n");
        return NULL;
    }


    if(!mem){
        void *new_mem = debug_malloc(size, file, line);
        return new_mem;
    }
        

    if(delete_allocation(mem)){
        printf("MemTrack ERROR: failed to free old tracking info for file %s, line - %d\n", file, line);
        return NULL;
    }
    
    void *new_mem = realloc(mem, size);
    debug_check_malloc_error(new_mem, file, line);

    if(!new_mem)
        return NULL;
    

    if(append_allocation(new_mem, file, line, size)){
        free(new_mem);
        printf("MemTrack ERROR: failed to malloc tracking info for file %s, line - %d\n", file, line);
        return NULL;            
    }


    return new_mem;
}

MemTrack_API char* debug_strdup(const char* src, char *file, int line){

    if(!file){
        printf("MemTrack ERROR: debug_free doesn't accept 'NULL' into 'char *file'\n");
        return NULL;
    }

    if(!src){
        printf("MemTrack ERROR: debug_strdup doesn't accept 'NULL' into 'char *src' for file %s, line - %d\n", file, line);
        return NULL;
    }

    size_t src_len = strlen(src);

    char *dup = debug_malloc(sizeof(char) * (src_len + 1), file, line);
    if(!dup)
        return NULL;

    strcpy(dup, src);
    dup[src_len] = '\0';

    return dup;
}