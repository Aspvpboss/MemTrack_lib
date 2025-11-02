

#include "MemTrack.h"
#include <stdio.h>
#include <stdlib.h>

void malloc_failure(void *data){
    printf("dub\n");
}

int main(void){

    Set_Malloc_Error_Function(malloc_failure, NULL);

    if(!t_malloc(929999923374)){
        
        return 1;
    }

    // int *array = t_malloc(-1);

    //array[2] = 1;

    // t_free(array);

    if(check_memory_leak())
        print_tracking_info();

    

    free_tracking_info();

    return 0;
}


