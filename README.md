
# Documentation

This library tracks allocations and can report memory leaks; furthermore, it can also
get heap usage and automatically null pointers after freeing them and exiting the program if malloc fails.

This library does support multi-threading, read the function docs for more info

I wrote this library in a way to configure it as you want. If you just want to track allocations when debugging, then only use the normal malloc and free during runtime you can do that. Or if you want it to null pointers when freed during runtime and check for malloc failure during runtime, in exchange for a very small performance hit.

#### when compiling use the these macros to configure the library. I use gcc so I use -Dmacro_name to define a global macro when compiling

### Compiler Macros 
- #define STDLIB_ALLOCATIONS - this uses stdlib malloc and free 

- #define TRACK_ALLOCATIONS - this uses my tracking malloc and free functions 

- #define NDEBUG - this will disable printf logging

- NO MACRO defined - this doesn't track allocations, however it still uses the safe malloc and free functions which allows for auto nulling pointers and exiting if malloc failure



####  There are macros that you can configure memtrack to use your own functions instead of the default ones it uses

### Function Macros
- #define TRACK_PRINTF(fmt, ...) printf(fmt, ##__VA_ARGS__)
- #define TRACK_MALLOC(size) malloc(size)
- #define TRACK_REALLOC(mem, newsize) realloc(mem, newsize)
- #define TRACK_FREE(ptr) free(ptr)
- #define TRACK_STRLEN(str) strlen(str)
- #define TRACK_STRCPY(dst, src) strcpy(dst, src)
- #define TRACK_STRDUP(str) strdup(str)
- #define TRACK_EXIT exit(EXIT_FAILURE)
- #define TRACK_MUTEX_TYPE pthread_mutex_t 
- #define TRACK_MUTEX_CREATE(mutex) pthread_mutex_init(&mutex, NULL)
- #define TRACK_MUTEX_LOCK(mutex) pthread_mutex_lock(mutex)
- #define TRACK_MUTEX_UNLOCK(mutex) pthread_mutex_unlock(mutex)

# Initializing

First, you must define the MEMTRACK_IMPLEMENTATION macro to paste the functions in, then include the memtrack header. You only need to define the MEMTRACK_IMPLEMENTATION once in one C file

    #define MEMTRACK_IMPLEMENTATION 
    #include "memtrack.h"

Secondly, in the main function, you need to call MemTrack_Init(); and use your desired configs

    MemTrack_Init(NULL, NULL, false, false);

If you are using the memory_failure_abort config, the malloc failure function you pass into the init function will run with the void* you pass into init if malloc fails

Importantly, if you're using the macro STDLIB_ALLOCATIONS, the auto_null_pointers and memory_failure_abort bools will do nothing


# USING MEMTRACK 

Here are the main functions and macros this library provides 

    FUNCTIONS

        size_t check_memory_usage(); // returns size_t of the amount of bytes used in heap
        int check_memory_leak(); // returns 1 if there are tracked allocations, returns 0 if there aren't tracked allocations
        void print_tracking_info(); // prints all tracking information


        // memtrack init and quit should be called only on the main thread after other threads using memtrack are cleaned up
        // if you call memtrack init more than once, it will just return 1 and nothing will happen
        int MemTrack_Init(void(*malloc_fail_handler)(void*), void *handler_arg, bool auto_null_pointers, bool memory_failure_abort);
        void MemTrack_Quit();

    MACROS 

        These macro definitions change depending on what global macro you define (you can look at the changes below in the header),
        but these are the generic args you give them

        void* t_malloc(size_t size);

        void* t_realloc(void *mem, size_t size);
        
        void t_free(void *mem); 

        char* t_strdup(const char *string);


EXAMPLE PROGRAM

    #define MEMTRACK_IMPLEMENTATION
    #include "memtrack.h"
    #include <stdio.h>


    void malloc_failure(void *data){
        printf("\ndub\n");
    }

    int main(void){

        MemTrack_Context ctx = {0};
        Set_MemTrack_Context(&ctx);
        ctx.config.auto_null_pointers = false;
        ctx.config.print_error_info = true;
        ctx.config.memory_failure_abort = true;

        Set_Malloc_Error_Function(malloc_failure, NULL);

        int *array = t_malloc(999999);

        if(!array)
            printf("is null\n");

        t_free(array);

        if(!array){

            printf("is null\n");

        } else{

            printf("isn't null\n");

        }
            

        if(check_memory_leak())
            print_tracking_info();

        free_tracking_info();

        return 0;
    }
