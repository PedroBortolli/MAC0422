#ifndef EP1_H
#define EP1_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <pthread.h>
#include <sys/time.h>

#define  MAXSIZE    150
#define  QUEUESIZE  150
#define  QUANTUM    2.0

#ifndef NOMINMAX

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#endif  /* NOMINMAX */

typedef struct Process {
    double t0, dt, deadline, start;
    char nome[MAXSIZE];
    int tid, line, cpuid;
} pr;

typedef struct ProcessQueue {
    pr* fila[QUEUESIZE];
    int size;
} pQueue;

typedef struct heap {
    int n, size;
    double *a;
    pr **pa;
} heap;

typedef struct ProcessNode {
    pr* p;
    struct ProcessNode * next;
} pNode;

typedef struct ProcessList {
    pNode* ini;
    pNode* fim;
    int size;
} pList;

// Variaveis globais

pthread_mutex_t * mutex;
pthread_mutex_t mutexLine;
int d = 0, nextLine = 1, lostDeadline = 0;
double tempo = 0.0;
FILE* out;

int comp(const void *a, const void* b) {
    const pr* c = *(pr**)a;
    const pr* d = *(pr**)b;
    double t1 = (double)c->t0;
    double t2 = (double)d->t0;
    if (t1 < t2) return -1;
    if (t1 == t2) return 0;
    else return 1;
}

double getTime() {
    struct timeval t;
    gettimeofday(&t, NULL);
    double tme = t.tv_sec * 1000000.0 + t.tv_usec;
    return tme/1000000.0;
}

double getElapsedTime(pr* p) {
    double now = getTime();
    double prev = p->start;
    return now-prev;
}

void writeFile(FILE* o, pr* p, double tempo, double tempoFinal) {
    fprintf(o, "%s %.1lf %.1lf\n", p->nome, tempo, tempoFinal);
}

void writeContext(FILE * o, int change) {
    fprintf(o, "%d\n", change);
}

void processFreedCPU(pr* p) {
    fprintf(stderr, "O processo '%s' liberou a CPU '%d'\n", p->nome, p->cpuid);
}

void processStartedCPU(pr *p) {
    fprintf(stderr, "O processo '%s' comecou a executar na CPU '%d'\n", p->nome, p->cpuid);
}

void processDone(pr *p, int line) {
    fprintf(stderr, "O processo '%s' foi finalizado e foi escrito na "
        "linha '%d' do arquivo de saida\n", p->nome, line);
}

void processArrived(pr *p) {
    fprintf(stderr, "O processo '%s' (linha '%d' do arquivo de trace) "
        "chegou ao sistema\n", p->nome, p->line);
}

void printLostDeadline() {
    fprintf(stderr, "Ao todo, '%d' processos foram finalizados apos sua "
        "deadline\n", lostDeadline);
}


void shortestJobFirst(pQueue* q);

void roundRobin(pQueue* q);

void priority(pQueue* q);

void insertProcess(pQueue* q, double t0, double dt, double deadline, char nome[], int l);

void pushProcess(pList* L, pr* p);

pr* removeProcess(pList* L);

pQueue* newProcessQueue();

pList* newProcessList();

heap* newHeap();

void heapfy(heap *h, int loc);

void insertHeap(heap *h, pr* p, int escal);

pr* removeHeap(heap *h);

#endif
