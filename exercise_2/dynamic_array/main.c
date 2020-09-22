#include <stdio.h>
#include "array.h"

int main(){
    Array arr = array_new(10);
    for(int i=0; i<50; i++){
        array_insertBack(&arr, i*i);
    }
    array_print_address(arr);
    array_destroy(arr);
    return 0;
}
