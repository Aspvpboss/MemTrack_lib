
#include "memtrack_test.h"



void malloc_failure(void *data){
    printf("\ndub\n");
}

int main(void){

    printf("C file\n----------------------\n");

    MemTrack_Context ctx = {0};
    Set_MemTrack_Context(&ctx);
    ctx.config.auto_null_pointers = false;
    ctx.config.print_error_info = true;
    ctx.config.memory_failure_abort = true;

    Set_Malloc_Error_Function(malloc_failure, NULL);

    int *array = t_malloc(999999);

    if(!array)
        printf("array is null\n");

    // array[2] = 1;

    // t_free(array);

    if(!array){

        printf("is null after free\n");

    } else{

        printf("isn't null after free\n");

    }
        

    if(check_memory_leak())
        print_tracking_info();

    

    free_tracking_info();

    return 0;
}


