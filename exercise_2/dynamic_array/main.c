#include <stdio.h>
#include "array.h"

int main(){
    Array arr = array_new(10);
    for(int i=0; i<31; i++){
        array_insertBack(&arr, i*i);
    }
    return 0;
}
