#ifndef BARREIRA_H
#define BARREIRA_H

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct barreira {
    int *arrive, *continua, *final;
    pthread_mutex_t *arrive_mutex, *continua_mutex;
    pthread_cond_t *arrive_cond, *continua_cond;
    int size, tempo, last;

} barreira;

barreira B;

//inicializa a barreira de tamanho n
void barreira_init(int n);

//executa a funcao do coordenador da barreira
void * barreira_coordenador();

//executa a funcao do ciclista na barreira
void barreira_ciclista(int i);

//estamos nas ultimas duas voltas
void barreira_last();

//guarda o tempo do ciclista no final
void barreira_final(int i, int volta);

//devolve o vetor dos tempos finais
int * barreira_tempo();

//destroi a barreira
void barreira_destroy();


#endif
