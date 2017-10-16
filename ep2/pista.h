#ifndef PISTA_H
#define PISTA_H

#include <stdio.h>
#include <stdlib.h>

int **pista;

//inicializa a pista
void pista_init(int d);

//set 1 o debug da pista
void pista_debug_set();

//imprime a pista no modo debug
void print_track();

//destroi a pista
void pista_destroy();

#endif
