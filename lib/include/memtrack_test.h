/* 
    Copyright (c) 2026 Benjamin Vaughan

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/


/*
    DOCUMENTATION

    This library tracks allocations and can report memory leaks; furthermore, it can also
    get heap usage and automatically null pointers after freeing them and exiting the program if malloc fails.

    As of right now this library doesn't support multi-threaded programs; however, multi-threading will be support soon

    I wrote this library in a way to configure it as you want. If you just want to track allocations when debugging, then
    only use the normal malloc and free during runtime you can do that. Or if you want it to null pointers when freed
    during runtime and check for malloc failure during runtime, in exchange for a very some performance hit.

    when compiling use the these macros to configure the library
    I use gcc so I use -Dmacro_name to define a global macro when compiling


    #define STDLIB_ALLOCATIONS - this uses stdlib malloc and free 

    #define TRACK_ALLOCATIONS - this uses my tracking malloc and free functions 

    NO MACRO defined - this doesn't track allocations, however it still uses my malloc and free functions 
                       which allows for auto nulling pointers and exiting if malloc failure


    INITIALIZING 

        First, you must define the MEMTRACK_IMPLEMENTATION macro to paste the functions in, then include the memtrack header.
        You only need to define the MEMTRACK_IMPLEMENTATION once in one C file

        Second, you must create a MemTrack_Context variable, either using malloc or the stack (whichever you prefer).
        Then, you must call the function Set_MemTrack_Context(), and pass in the pointer of the defined MemTrack_Context.

        After that you can set the specific configs of memtrack using the MemTrack_Context. 
        
        These are the vars you can configs in MemTrack_Context, Importantly, the configs only work
        if you aren't using STDLIB_ALLOCATIONS so be aware.

            bool memory_failure_abort; if true, will exit program if malloc fails
            bool auto_null_pointers; if true, will automatically set freed pointers to null
            bool print_error_info; if true, will print debug info with memtrack_print functions

        Optionally, if the memory_failure_abort config is true, you can set a function to run before the program
        exit. Use the Set_Malloc_Error_Function() to set the function and data, it won't fail or crash if memory_failure_abort
        is false

        Set_Malloc_Error_Function() takes a functions pointer that returns void and takes a void* as a parameter, then 
        the void *data 

        Internally when the function is called, it passes the void *data into the function

    
    USING MEMTRACK 

    Here are the main functions and macros this library provides 
    
        FUNCTIONS

            size_t check_memory_usage(); // returns size_t of the amount of bytes used in heap
            int check_memory_leak(); // returns 1 if there are tracked allocations, returns 0 if there aren't tracked allocations
            void print_tracking_info(); // prints all tracking information
            void free_tracking_info(); // should always be called at the end of a program to free tracking_info

            //if you don't call this, then no functions will be called if malloc returns NULL
            void Set_Malloc_Error_Function(void(*function)(void*), void *function_arg);

            //you must call this *before* you set the values within MemTrack_Context because it zeros all values
            void Set_MemTrack_Context(MemTrack_Context *e_ctx);

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

*/



#ifndef MEMTRACK_test_H
#define MEMTRACK_test_H 


#ifdef __cplusplus 
extern "C"{
#endif



#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

#define TRACK_MALLOC(size) malloc(size)
#define TRACK_REALLOC(mem, newsize) realloc(mem, newsize)
#define TRACK_FREE(ptr) free(ptr)
#define TRACK_PRINTF(fmt, ...) printf(fmt, ##__VA_ARGS__)
#define TRACK_STRLEN(str) strlen(str)
#define TRACK_STRCPY(dst, src) strcpy(dst, src)
#define TRACK_STRDUP(str) strdup(str)
#define TRACK_EXIT exit(EXIT_FAILURE)

typedef struct Mem_Info{

    struct Mem_Info *next;
    size_t size;
    void *ptr;
    char *file_name;
    int file_line;

} Mem_Info;



size_t check_memory_usage();
void print_tracking_info();
void free_tracking_info();
int check_memory_leak();




//if you don't call this, then no functions will be called if malloc returns NULL
void Set_Malloc_Error_Function(void(*function)(void*), void *function_arg);

//you must call this *before* you set the values within MemTrack_Context
void Set_MemTrack_Context(MemTrack_Context *e_ctx);

void safe_free(void **mem);
void* safe_malloc(size_t size);
void* safe_realloc(void *memory, size_t size);
char* safe_strdup(const char *src);

void debug_free(void **mem, char *file, int line);
void* debug_malloc(size_t size, char *file, int line);
void* debug_realloc(void *mem, size_t size, char *file, int line);
char* debug_strdup(const char* src, char *file, int line);



#ifdef TRACK_ALLOCATIONS

    //void *t_malloc(size_t size);
    #define t_malloc(size) debug_malloc(size, __FILE__, __LINE__)

    //void *t_realloc(void *ptr, size_t size);
    #define t_realloc(ptr, size) debug_realloc(ptr, size, __FILE__, __LINE__)

    /*
    void t_free(void **mem);
    */
    #define t_free(ptr) debug_free((void**)&ptr, __FILE__, __LINE__)

    //char *t_strdup(char *str);
    #define t_strdup(str) debug_strdup(str, __FILE__, __LINE__)
 
#elif defined(STDLIB_ALLOCATIONS)
 
    //void *t_malloc(size_t size);
    #define t_malloc(size) malloc(size)

    //void *t_realloc(void *ptr, size_t size);
    #define t_realloc(ptr, size) realloc(ptr, size)

    /*
    void free(void *mem);
    */
    #define t_free(ptr) free(ptr)

    //char *t_strdup(char *str);
    #define t_strdup(str) strdup(str)

#else

    //void *t_malloc(size_t size);
    #define t_malloc(size) safe_malloc(size)

    //void *t_realloc(void *mem, size_t size);
    #define t_realloc(ptr, size) safe_realloc(ptr, size)

    /*
    void t_free(void **mem);
    auto NULL's pointer
    */
    #define t_free(ptr) safe_free((void**)&ptr)

    //char *t_strdup(char *str);
    #define t_strdup(str)  safe_strdup(str)

#endif

#ifdef __cplusplus
}
#endif



#endif
