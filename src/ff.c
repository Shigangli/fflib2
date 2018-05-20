
#include "ff.h"
#include "ffop.h"
#include "ffprogress.h"
#include "ffinternal.h"

#include "mpi/ffmpi.h"

static ffdescr_t ff;

int ffinit(int * argc, char *** argv){

    int ret; 

    ffop_init();
    ff.terminate = 0;
    ret = pthread_create(&(ff.progress_thread), NULL, progress_thread, &ff);
    if (ret){ return FFERROR; }

    ff.impl_init = ffmpi_init;
    ff.impl_finalize = ffmpi_finalize;
    ff.impl_get_rank = ffmpi_get_rank;
    ff.impl_get_size = ffmpi_get_size;

    ff.impl_init(argc, argv);

    return FFSUCCESS;
}


int fffinalize(){

    ff.terminate=1;
    if (pthread_join(ff.progress_thread, NULL)){
        return FFERROR;
    }
 
    ff.impl_finalize();   
    return FFSUCCESS;
}

int ffrank(int * rank){
    return ff.impl_get_rank(rank);
}

int ffsize(int * size){
    return ff.impl_get_size(size);
}

