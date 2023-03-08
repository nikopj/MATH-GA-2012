#include <stdio.h>
#include <omp.h>

int main(int argc, char** argv){
    #pragma omp parallel
    {
        printf("Hello from process: %d\n", omp_get_thread_num());
    }

    #pragma omp parallel for
    for(int i=0; i<10; i++)
    {
        printf("%d: Hello from process: %d\n", i, omp_get_thread_num());
    }
    return 0;
}
