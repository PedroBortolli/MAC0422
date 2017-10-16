/*###########################################
##                                         ##
##  EP2 - MAC0422 - Sistemas Operacionais  ##
##  Pedro Vítor Bortolli Santos - 9793721  ##
##  Jonas Arilho Levy - 9344935            ## 
##                                         ##
###########################################*/


#include "barreira.h"
#include "pista.h"

void barreira_init(int n) {

    B.arrive   = calloc(n, sizeof(int));
    B.continua = calloc(n, sizeof(int));
    B.final    = calloc(n, sizeof(int));
    B.arrive_mutex    = calloc(n, sizeof(pthread_mutex_t));
    B.continua_mutex  = calloc(n, sizeof(pthread_mutex_t));
    B.arrive_cond     = calloc(n, sizeof(pthread_cond_t));
    B.continua_cond   = calloc(n, sizeof(pthread_cond_t));
    B.size = n;
    B.tempo = 0;
    B.last = 0;

    int i;
    for (i = 0; i < n; i++) {
        pthread_mutex_init(&B.arrive_mutex[i], NULL);
        pthread_mutex_init(&B.continua_mutex[i], NULL);
        pthread_cond_init(&B.arrive_cond[i], NULL);
        pthread_cond_init(&B.continua_cond[i], NULL);
    }

}

void * barreira_coordenador() {

    int i, stop = 1, n = B.size;

    while (stop) {
        stop = 0;

        //ciclistas esperam pelo sinal
        for (i = 0; i < n; i++) {
            if (B.final[i] == 0) {
                stop = 1;
                pthread_mutex_lock(&B.arrive_mutex[i]);
                while (B.arrive[i] == 0) {
                    pthread_cond_wait(&B.arrive_cond[i], &B.arrive_mutex[i]);
                }
                B.arrive[i] = 0;
                pthread_mutex_unlock(&B.arrive_mutex[i]);
            }
        }
        if (B.last) B.tempo++;
        else B.tempo += 3;

        //==== DEBUG =====
        print_track();


        //ciclistas podem continuar
        for (i = 0; i < n; i++) {
            pthread_mutex_lock(&B.continua_mutex[i]);
            B.continua[i] = 1;
            pthread_cond_signal(&B.continua_cond[i]);
            pthread_mutex_unlock(&B.continua_mutex[i]);
        }
    }
}

void barreira_ciclista(int i) {
    //ciclista envia sinal
    pthread_mutex_lock(&B.arrive_mutex[i]);
    B.arrive[i] = 1;
    pthread_cond_signal(&B.arrive_cond[i]);
    pthread_mutex_unlock(&B.arrive_mutex[i]);

    //ciclistas esperam para continuar
    pthread_mutex_lock(&B.continua_mutex[i]);
    while (B.continua[i] == 0) {
        pthread_cond_wait(&B.continua_cond[i], &B.continua_mutex[i]);
    }
    B.continua[i] = 0;
    pthread_mutex_unlock(&B.continua_mutex[i]);

}

void barreira_last() {
    B.last = 1;
}

void barreira_final(int i, int volta) {

    if (volta) B.final[i] = volta;
    else B.final[i] = B.tempo*20;

    pthread_mutex_lock(&B.arrive_mutex[i]);
    B.arrive[i] = 1;
    pthread_cond_signal(&B.arrive_cond[i]);
    pthread_mutex_unlock(&B.arrive_mutex[i]);
}

int * barreira_tempo() {

    return B.final;
}


void barreira_destroy() {

    int i, n = B.size;

    for (i = 0; i < n; i++) {
        pthread_mutex_destroy(&B.arrive_mutex[i]);
        pthread_mutex_destroy(&B.continua_mutex[i]);
    }

    free(B.arrive);
    free(B.continua);
    free(B.final);
    free(B.arrive_mutex);
    free(B.continua_mutex);
    free(B.arrive_cond);
    free(B.continua_cond);

}
