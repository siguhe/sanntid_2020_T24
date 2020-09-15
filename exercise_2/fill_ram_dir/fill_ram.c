#include <stdio.h>
int main(){
    long xy_size = 1000*1000*1000; // 8GB (sizeof(long) = 8 bytes)
    long x_dim = 10000;
    long y_dim = xy_size/x_dim;

    long** matrix = malloc(y_dim*sizeof(long*));

    for( long y = 0; y < y_dim; y++){
        matrix[y] = malloc(x_dim*sizeof(long));
        memset(matrix[y], 0, x_dim*sizeof(long));
    }

    printf("Allocation complete\n");
    getchar();
    return 0;
}
