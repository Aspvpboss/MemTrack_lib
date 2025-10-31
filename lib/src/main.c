

#include "MemTrack.h"
#include <stdio.h>
#include <stdlib.h>

int main(void){

    if(!malloc(9223372036854)){
        printf("dub\n");
    }

    // int *array = t_malloc(-1);

    //array[2] = 1;

    // t_free(array);

    if(check_memory_leak())
        print_tracking_info();

    

    free_tracking_info();

    return 0;
}


