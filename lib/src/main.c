

#include "MemTrack.h"
#include <stdio.h>

int main(void){

    int *array = t_malloc(sizeof(int) * 10);

    array[2] = 1;


    if(check_memory_leak())
        print_tracking_info();

    t_free(array);

    free_tracking_info();

    return 0;
}


