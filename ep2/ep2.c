/*###########################################
##                                         ##
##  EP2 - MAC0422 - Sistemas Operacionais  ##
##  Pedro Vítor Bortolli Santos - 9793721  ##
##  Jonas Arilho Levy - 9344935            ## 
##                                         ##
###########################################*/


#include "ep2.h"
#include "pista.h"
#include "barreira.h"

//Speed 1 = rapido, 60km/h
//Speed 2 = devagar, 30km/h

void slow_down(int id) {
	if (!last_two) {
		ciclista[id].speed = 2;
		ciclista[id].step = ciclista[id].speed;
	}
	else {
		ciclista[id].speed = 6;
		ciclista[id].step = ciclista[id].speed;
	}
}

int get_new_speed(int old_speed) {
	int x = rand()%10 + 1; //gera um numero aleatorio entre 1 e 10
	if (old_speed == 1) {
		if (x <= 7) return 1;
		else return 2;
	}
	else {
		if (x <= 5) return 1;
		else return 2;
	}
}

void recalc() {
	//speed = 1  60ms   vai virar 3   simulando a 20ms
	//speed = 2  120ms  vai virar 6   simulando a 20ms
	for (int i = 0; i < n; i++) {
		if (ciclista[i].speed == 1) {
			ciclista[i].speed = 3;
			ciclista[i].step = ciclista[i].speed;
		}
		else {
			ciclista[i].speed = 6;
			ciclista[i].step = ciclista[i].speed;
		}
	}
}

void random_boost() {
	int x = rand()%n;
	int y = rand()%10+1;
	if (y == 1) {
		ciclista[x].speed = 2;
		ciclista[x].step = ciclista[x].speed;
	}
}

void check_for_points(int id) {
	int lap = ciclista[id].lap, ok = 1;

	for (int i = 0; i < n && ok; i++) {
		if (i == id) continue;
		if (ciclista[i].lap >= (lap - 1 - ciclista[id].mito)) ok = 0;
	}
	if (ok) {
		ciclista[id].mito++;
		ciclista[id].score += 20;
	}
}

void move(int id, int to_pos, int to_raia) {

	pista[to_pos][to_raia] = id;
    pthread_mutex_unlock(&pista_mutex[to_pos]);

    pthread_mutex_lock(&pista_mutex[ciclista[id].pos]);
    pista[ciclista[id].pos][ciclista[id].raia] = -1;
    pthread_mutex_unlock(&pista_mutex[ciclista[id].pos]);

	ciclista[id].pos = to_pos;
	ciclista[id].raia = to_raia;
	if (ciclista[id].lap && to_pos == 0) {
		ciclista[id].speed = get_new_speed(ciclista[id].speed);
		ciclista[id].step = ciclista[id].speed;
        int lap = ciclista[id].lap;

        pthread_mutex_lock(&rank_mutex[lap]);
		rank[lap]++;

        if (lap > 1 && rank[lap] == 1) check_for_points(id);

		if (lap > 0 && lap%10 == 0) {
			if (rank[lap] == 1) ciclista[id].score += 5;
			if (rank[lap] == 2) ciclista[id].score += 3;
			if (rank[lap] == 3) ciclista[id].score += 2;
			if (rank[lap] == 4) ciclista[id].score += 1;

            pontos[lap][id] = ciclista[id].score;

            if((relat || lap == v) &&  (rank[lap] == (n - broke))){
                printf("==> volta %d\n", lap);
                for (int i = 0; i < n; i++) {
                    standings[i].id = i;
                    standings[i].score = pontos[lap][i];
                    if (standings[i].score == -1000000000) standings[i].score = 0;
                }
                qsort(standings, n, sizeof(pair*), comp);
                for (int i = 0; i < n; i++) {
                    printf("=> posicao %d => ciclista %d = ", i+1, standings[i].id);
                    if (standings[i].score < 0) printf("esta quebrado com %d ponto(s)\n", standings[i].score * (-1));
                    else printf("%d ponto(s)\n", standings[i].score);
                }
            }
		}
        else if (lap < v) {
            pontos[lap][rank[lap]-1] = id;

            if((relat || lap == v) &&  (rank[lap] == (n - broke))){
                printf("==> volta %d\n", lap);
                for (int i = 0; i < n - broke; i++)
                    printf("=> posicao %d => ciclista %d\n", i+1, pontos[lap][i]);
            }
        }

        pthread_mutex_unlock(&rank_mutex[ciclista[id].lap++]);

		if (ciclista[id].lap == v-1 && !last_two) {
			last_two = 1;
            barreira_last();
			recalc();
			random_boost();
		}
	}
	if (!ciclista[id].lap && to_pos == d-1)
		ciclista[id].lap = 1;

}

int break_biker(int id) {
	int x = rand()%100;
	if (!x) {
		if (relat) printf("O ciclista %d quebrou\n", id);
		return 1;
	}
	return 0;
}

void * exec(void* args) {
	struct Ciclistas *arg = (struct Ciclistas *)args;

	int id = arg->id;

    while (ciclista[id].lap <= v) {

    	if (ciclista[id].lap%15 == 0 && ciclista[id].lap > 0 && ciclista[id].pos == n-1 && n-broke > 5) {
    		if (break_biker(id)) {

    			pthread_mutex_lock(&pista_mutex[ciclista[id].pos]);
			    pista[ciclista[id].pos][ciclista[id].raia] = -1;
			    pthread_mutex_unlock(&pista_mutex[ciclista[id].pos]);

			    barreira_final(id, ciclista[id].lap);

                ciclista[id].lap = -1;
                if (ciclista[id].lap == 0) ciclista[id].lap = -1000000000;
                else ciclista[id].score *= -1;
			    broke++;

                for (int i = ciclista[id].lap + (10 - ciclista[id].lap%10); i <= v; i += 10) {
                    pontos[i][id] = ciclista[id].score;
                }

    			return NULL;
    		}
    	}

    	ciclista[id].step--;
    	if (ciclista[id].step <= 0) {
    		//mexe esse cara pra frente
    		int next = ciclista[id].pos - 1;
    		if (next < 0) next = d-1;

            pthread_mutex_lock(&pista_mutex[next]);

    		if (pista[next][ciclista[id].raia] != -1) {
    			if (ciclista[id].raia < 9 && pista[next][ciclista[id].raia + 1] == -1) {
                    move(id, next, ciclista[id].raia+1);
                }
    			else {
                    pthread_mutex_unlock(&pista_mutex[next]);
                    slow_down(id);
                }
    		}

    		else if (ciclista[id].raia > 0 && pista[next][ciclista[id].raia-1] == -1) {
    			move(id, next, ciclista[id].raia-1);
    		}

    		else move(id, next, ciclista[id].raia);
    	}
        barreira_ciclista(id);
    }

    pthread_mutex_lock(&pista_mutex[ciclista[id].pos]);
    pista[ciclista[id].pos][ciclista[id].raia] = -1;
    pthread_mutex_unlock(&pista_mutex[ciclista[id].pos]);

    barreira_final(id, 0);

}

void sim() {

    int i;
    for (i = 0; i < n; i++)
        pthread_mutex_unlock(&ciclista[i].init_mutex);

}

void init() {
    ciclista = (ciclistas*)malloc(sizeof(ciclistas) * n);
    rank = calloc(v+1, sizeof(int));
    pista_mutex  = calloc(d, sizeof(pthread_mutex_t));
    rank_mutex = calloc(v+1, sizeof(pthread_mutex_t));

    standings = malloc(n * sizeof(pair));

    pontos = (int**)malloc((v+1) * sizeof(int*));
    for (int i = 0; i <= v; i++) {
		pontos[i] = malloc(n * sizeof(int));
	}

    for (int i = 0; i <= v; i++) {
		for (int j = 0; j < n; j++) {
			pontos[i][j] = -1;
		}
	}

    pista_init(d);

    barreira_init(n);

    pthread_create(&coordenador, NULL, barreira_coordenador, NULL);

    for (int i = 0; i < d; i++)
        pthread_mutex_init(&pista_mutex[i], NULL);

    for (int i = 0; i < v; i++)
        pthread_mutex_init(&rank_mutex[i], NULL);


	int biker = 0;
	for (int i = 0; i < d && biker < n; i++) {
		for (int j = 0; j < 10 && biker < n; j++) {
			pista[i][j] = biker;
			ciclista[biker].id = biker;
			ciclista[biker].pos = i;
			ciclista[biker].raia = j;
			ciclista[biker].speed = 2;
			ciclista[biker].step = 2;
			ciclista[biker].lap = 0;
			ciclista[biker].score = 0;
            ciclista[biker].mito = 0;
            pthread_mutex_init(&ciclista[biker].init_mutex, NULL);
			pthread_mutex_lock(&ciclista[biker].init_mutex);
			pthread_create(&ciclista[biker].t, NULL, exec, &ciclista[biker]);
			biker++;
		}
	}
}

/* “Now I am become death, the destroyer of worlds”
is a quote from scientist J. Robert Oppenheimer
theoretical physicist and credited father of the atomic bomb*/
void death() {

    int i;

    for (i = 0; i < n; i++)
        pthread_join(ciclista[i].t, NULL);

    int * final = barreira_tempo();

    printf("== TEMPOS FINAIS ==\n");

    for (i = 0; i < n; i++) {
        printf("Ciclista %d => ", i);
        if(pontos[v][i] < 0) printf("quebrou na volta %d\n", final[i]);
        else printf("tempo = %d ms\n", final[i]);
    }


    barreira_destroy();

    for (i = 0; i < n; i++) {
        pthread_mutex_destroy(&ciclista[i].init_mutex);
    }

    for (i = 0; i < d; i++) {
        pthread_mutex_destroy(&pista_mutex[i]);
    }

    for (i = 0; i <= v; i++) {
        pthread_mutex_destroy(&rank_mutex[i]);
        free(pontos[i]);
    }

    free(ciclista);
    free(standings);
    free(rank);
    free(pontos);
    free(pista_mutex);
    free(rank_mutex);

}

int main(int argc, char const *argv[]) {
	srand(time(NULL));
	d = atoi(argv[1]);
	n = atoi(argv[2]);
	v = atoi(argv[3]);
	if (argc == 5 && !strcmp(argv[4], "-d")) pista_debug_set();
    if (argc == 5 && !strcmp(argv[4], "-r")) relat = 0;
	init();
	sim();
    death();

	return 0;
}
