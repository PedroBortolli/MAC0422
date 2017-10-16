/*###########################################
##                                         ##
##  EP2 - MAC0422 - Sistemas Operacionais  ##
##  Pedro Vítor Bortolli Santos - 9793721  ##
##  Jonas Arilho Levy - 9344935            ## 
##                                         ##
###########################################*/


#include "pista.h"

int db = 0, dis;

void pista_init(int d) {
    dis = d;
    pista = (int**)malloc(sizeof(int*) * d);
	for (int i = 0; i < d; i++) {
		pista[i] = (int*)malloc(10*sizeof(int));
	}

	for (int i = 0; i < d; i++) {
		for (int j = 0; j < 10; j++) {
			pista[i][j] = -1;
		}
	}
}

void pista_debug_set() {
    db = 1;
}

void print_track() {
    if(!db) return;

	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < dis; j++) {
			printf("%d	", pista[j][i]);
		}
		printf("\n");
	}
	printf("\n");
}

void pista_destroy() {
    for (int i = 0; i < dis; i++) {
        free(pista[i]);
    }
    free(pista);
}
