#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#include "ff.h"

#define FFCALL(X) { int ret; if (ret=(X)!=FFSUCCESS) { printf("Error: %i\n", ret); exit(-1); } }

#define N 2

int main(int argc, char * argv[]){
    
    int rank, size, count;

    if (argc!=2){
        printf("Usage: %s <count>\n", argv[0]);
        exit(1);
    } 

    count = atoi(argv[1]);

    ffinit(&argc, &argv);

    ffrank(&rank);
    ffsize(&size);

    int32_t to_reduce = 5;
    int32_t reduced = 0;
    
    int failed=0;
    
    ffschedule_h allreduce;
    ffallreduce(&to_reduce, &reduced, 1, 0, FFSUM, FFINT32, &allreduce);

    MPI_Barrier(MPI_COMM_WORLD); //not needed, just for having nice output
    for (int i=0; i<N && !failed; i++){
        reduced=0;

        printf("Rank %i; POST\n", rank);

        ffschedule_post(allreduce);
        ffschedule_wait(allreduce);

        printf("Rank %i reduced value: %i\n", rank, reduced);
        if (reduced != size*to_reduce){
            printf("FAILED!\n");
            failed=1;
        }

        /* this is ugly... TODO: have internal tags for collectives or allow to change the tag of an operation without changing it */
        MPI_Barrier(MPI_COMM_WORLD);
    }

    ffschedule_delete(allreduce);

    fffinalize();

    if (!failed){
        printf("PASSED!\n");
    }
    
    return 0;
}
