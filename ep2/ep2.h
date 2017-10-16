#ifndef EP2_H
#define EP2_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

typedef struct Ciclistas {
	pthread_t t;
    pthread_mutex_t init_mutex;
	int id, pos, raia, speed, step, lap, score, mito;
} ciclistas;

typedef struct Pair {
    int id, score;
} pair;

ciclistas* ciclista;
int *rank, **pontos;
pair *standings;
pthread_mutex_t *pista_mutex, *rank_mutex;
pthread_t coordenador;
int d, n, v, last_two = 0, relat = 1, broke = 0;

//para uso do qsort()
int comp(const void *a, const void *b) {
    const pair c = *(pair*)a;
    const pair d = *(pair*)b;
    int t1 = (int)c.score;
    int t2 = (int)d.score;
    if (t1 > t2) return -1;
    if (t1 == t2) return 0;
    else return 1;
}

//diminui a velocidade
void slow_down(int id);

//muda a velocidade do ciclista
int get_new_speed(int old_speed);

//recalcula a velocidade do ciclista
void recalc();

//velocidade maior
void random_boost();

//ve se o ciclista eh mito
void check_for_points(int id);

//ciclista faz o movimento
void move(int id, int to_pos, int to_raia);

//ciclista quebra
int break_biker(int id);

//ciclista executa esta funcao
void * exec(void* args);

//simulacao
void sim();

//inicializa os ciclistas
void init();

//morre tudo
void death();

#endif
