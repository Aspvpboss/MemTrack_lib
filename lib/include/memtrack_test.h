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



#ifndef MEMTRACK_test_H
#define MEMTRACK_test_H 


#ifdef __cplusplus 
extern "C"{
#endif


// uncomment this for use in DLLs
// #define MEMTRACK_DLL

#if defined(MEMTRACK_DLL)

    #if defined(_WIN32)
        #if defined(MEMTRACK_BUILD_DLL)
            #define MEMTRACK_API __declspec(dllexport)
        #else
            #define MEMTRACK_API __declspec(dllimport)
        #endif
    #else
        #define MEMTRACK_API __attribute__((visibility("default")))
    #endif

#else
    #define MEMTRACK_API
#endif


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

// changes these macros to use what functions you want, be careful

#ifndef NDEBUG
    #define TRACK_PRINTF(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else 
    #define TRACK_PRINTF(fmt, ...)
#endif


#define TRACK_MALLOC(size) malloc(size)
#define TRACK_REALLOC(mem, newsize) realloc(mem, newsize)
#define TRACK_FREE(ptr) free(ptr)
#define TRACK_STRLEN(str) strlen(str)
#define TRACK_STRCPY(dst, src) strcpy(dst, src)
#define TRACK_STRDUP(str) strdup(str)
#define TRACK_EXIT exit(EXIT_FAILURE)

#define TRACK_MUTEX_TYPE pthread_mutex_t 
#define TRACK_MUTEX_CREATE(mutex) pthread_mutex_init(&mutex, NULL)
#define TRACK_MUTEX_DESTROY(mutex) pthread_mutex_destroy(&mutex)
#define TRACK_MUTEX_LOCK(mutex) pthread_mutex_lock(&mutex)
#define TRACK_MUTEX_UNLOCK(mutex) pthread_mutex_unlock(&mutex)

typedef struct Mem_Info{

    struct Mem_Info *next;
    size_t size;
    void *ptr;
    char *file_name;
    int file_line;

} Mem_Info;



MEMTRACK_API size_t check_memory_usage();
MEMTRACK_API void print_tracking_info();
MEMTRACK_API int check_memory_leak();

// init and quit should only be called on main thread
MEMTRACK_API void MemTrack_Quit();
MEMTRACK_API int MemTrack_Init(void(*malloc_fail_handler)(void*), void *handler_arg, bool auto_null_pointers, bool memory_failure_abort);


MEMTRACK_API void safe_free(void **mem);
MEMTRACK_API void* safe_malloc(size_t size);
MEMTRACK_API void* safe_realloc(void *memory, size_t size);
MEMTRACK_API char* safe_strdup(const char *src);

MEMTRACK_API void debug_free(void **mem, const char *file, int line);
MEMTRACK_API void* debug_malloc(size_t size, const char *file, int line);
MEMTRACK_API void* debug_realloc(void *mem, size_t size, const char *file, int line);
MEMTRACK_API char* debug_strdup(const char* src, const char *file, int line);



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
