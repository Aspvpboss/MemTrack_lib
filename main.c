

#include "MemTrack.h"
#include <stdio.h>
#include <stdlib.h>

void malloc_failure(void *data){
    printf("\ndub\n");
}

int main(void){

    MemTrack_Context ctx = {0};
    Set_MemTrack_Context(&ctx);
    ctx.config.auto_null_pointers = true;
    ctx.config.print_error_info = true;
    ctx.config.memory_failure_abort = true;
    Set_Malloc_Error_Function(malloc_failure, NULL);



    int *array = t_malloc(99999999999999);

    array[2] = 1;

    t_free(array);

    if(!array)
        printf("is null\n");

    if(check_memory_leak())
        print_tracking_info();

    

    free_tracking_info();

    return 0;
}


