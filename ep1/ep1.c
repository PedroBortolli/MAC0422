#include "ep1.h"

void* exec(void* args) {
    struct Process *arg = (struct Process *)args;
    int id = arg->tid;

    while (1) {
        pthread_mutex_lock(&mutex[id]);
        double tme = getElapsedTime(arg);
        if (tme >= arg->dt) break;
        pthread_mutex_unlock(&mutex[id]);
    }

    double elapsed = tempo-arg->t0;
    writeFile(out, arg, tempo, elapsed);
    if (tempo > arg->deadline) lostDeadline++;
    if (d) {
        processFreedCPU(arg);
        pthread_mutex_lock(&mutexLine);
        processDone(arg, nextLine);
        pthread_mutex_unlock(&mutexLine);
    }
    nextLine++;
    return 0;
}

double getQuantum(pr * p) {
    //pega a razao entre o tempo atual da simulacao e a deadline do processo p
    //quanto menor for esta razao, mais longe esse processo esta de seu
    //termino maximo permitido, e portanto o quantum recebido eh menor
    double ratio = tempo/p->deadline;
    if (ratio < 0.2) return min(p->dt, 0.5);
    if (ratio < 0.4) return min(p->dt, 1.0);
    if (ratio < 0.6) return min(p->dt, 1.5);
    if (ratio < 0.8) return min(p->dt, 2.0);
    if (ratio < 1.0) return min(p->dt, 2.5);
    return min(p->dt, 3.0);
}

void shortestJobFirst(pQueue* q) {
    int cont = 0, toProcess = q->size, changeContext = 0;
    pr* top;
    heap* H = newHeap();
    int cores = (int)sysconf(_SC_NPROCESSORS_ONLN);
    mutex = (pthread_mutex_t *)malloc(toProcess*sizeof(pthread_mutex_t));
    pthread_t * thread = (pthread_t *)malloc(toProcess*sizeof(pthread_t));
    double * end = (double *)malloc(cores*sizeof(double));
    int * map = (int *)malloc(cores*sizeof(int));
    tempo = 0.0;

    for (int i = 0; i < cores; i++) {
        end[i] = -1.0;
        map[i] = -1;
    }

    while (toProcess) {

        usleep(100000);

        while (cont < q->size && fabs(tempo - q->fila[cont]->t0) < 0.01) {
            if (d) processArrived(q->fila[cont]);
            insertHeap(H, q->fila[cont], 1);
            pthread_mutex_init(&mutex[cont], NULL);
            pthread_mutex_lock(&mutex[cont]);
            pthread_create(&thread[cont], NULL, exec, (void *)q->fila[cont]);
            cont++;
        }

        for (int i = 0; i < cores; i++) {
            if (end[i] > -1.0 && fabs(tempo-end[i]) < 0.01) {
                toProcess--;
                end[i] = -1.0;
                map[i] = -1;
            }
        }

        for (int i = 0; i < cores && H->n > 0; i++) {
            if (end[i] == -1.0) {
                top = removeHeap(H);
                end[i] = tempo + top->dt;
                top->start = getTime();
                map[i] = top->tid;
                top->cpuid = i+1;
                if (d) processStartedCPU(top);
                pthread_mutex_unlock(&mutex[map[i]]);
            }
        }

        tempo += 0.1;
    }

    if (d) printLostDeadline();
    writeContext(out, changeContext);
    free(thread);
    free(mutex);
    free(end);
    free(map);
}

void roundRobin(pQueue* q) {

    pList *waiting = newProcessList(), *running = newProcessList();

    int cont = 0, sz = q->size, toProcess = q->size, id, changeContext = 0;
    int cores = (int)sysconf(_SC_NPROCESSORS_ONLN);
    mutex = (pthread_mutex_t *)malloc(sz*sizeof(pthread_mutex_t));
    pthread_t * thread = (pthread_t *)malloc(sz*sizeof(pthread_t));
    int * map = (int *)malloc(cores*sizeof(int));
    const double quantum = QUANTUM;
    double qt = quantum;
    pr* atual;
    tempo = 0.00;

    for (int i = 0; i < cores; i++) map[i] = -1;

    while (toProcess) {

        usleep(100000);

        while (cont < sz && fabs(tempo - q->fila[cont]->t0) < 0.01) {
            if (d) processArrived(q->fila[cont]);
            atual = q->fila[cont++];
            id = atual->tid;
            atual->dt = max(atual->dt, quantum);
            pushProcess(waiting, atual);
            pthread_mutex_init(&mutex[id], NULL);
            pthread_mutex_lock(&mutex[id]);
            pthread_create(&thread[id], NULL, exec, (void *)atual);
        }

        if (fabs(qt - tempo) < 0.01) {
            for (int i = 0; i < cores && running->size; i++) {
                if ((map[i] >= 0) && (fabs(tempo) > 0.01)) {
                    atual = removeProcess(running);
                    double dt = atual->dt - quantum;
                    if (dt > 0.01) {
                        changeContext++;
                        pthread_mutex_lock(&mutex[atual->tid]);
                        if (d) processFreedCPU(atual);
                        atual->dt = max(dt, quantum);
                        pushProcess(waiting, atual);
                    }
                    else toProcess--;
                }
            }
            for (int i = 0; i < cores && waiting->size; i++) {
                atual = removeProcess(waiting);
                map[i] = atual->tid;
                atual->start = getTime();
                atual->cpuid = i+1;
                pushProcess(running, atual);
                if (d) processStartedCPU(atual);
                pthread_mutex_unlock(&mutex[atual->tid]);
            }
            qt += quantum;
        }

        tempo += 0.1;
    }

    if (d) printLostDeadline();
    writeContext(out, changeContext);
    free(thread);
    free(mutex);
    free(map);
    free(running);
    free(waiting);
}


void priority(pQueue * q) {

    pList *waiting = newProcessList();

    int cont = 0, toProcess = q->size, changeContext = 0;
    pr* busy;
    mutex = (pthread_mutex_t *)malloc(toProcess*sizeof(pthread_mutex_t));
    pthread_t * thread = (pthread_t *)malloc(toProcess*sizeof(pthread_t));
    int map = -1;
    double end = -1.0, next = -1.0, quantum = 0.0;
    tempo = 0.0;

    while (toProcess) {

        usleep(100000);
        while (cont < q->size && fabs(tempo - q->fila[cont]->t0) < 0.01) {
            if (fabs(next+1.0) < 0.01) next = q->fila[cont]->t0;
            if (d) processArrived(q->fila[cont]);
            pushProcess(waiting, q->fila[cont]);
            pthread_mutex_init(&mutex[cont], NULL);
            pthread_mutex_lock(&mutex[cont]);
            pthread_create(&thread[cont], NULL, exec, (void *)q->fila[cont]);
            cont++;
        }

        if (fabs(tempo-next) < 0.01) {
            if (end > -1.0) {
                if (fabs(tempo-end) < 0.01) {
                    toProcess--;
                    end = -1.0;
                    next = -1.0;
                }
                else {
                    pthread_mutex_lock(&mutex[busy->tid]);
                    if (d) processFreedCPU(busy);
                    pushProcess(waiting, busy);
                    busy->dt -= quantum;
                    changeContext++;
                }
            }
            if (waiting->size > 0) {
                busy = removeProcess(waiting);
                quantum = getQuantum(busy);
                next = tempo+quantum;
                end = tempo+busy->dt;
                busy->start = getTime();
                busy->cpuid = 1;
                if (d) processStartedCPU(busy);
                pthread_mutex_unlock(&mutex[busy->tid]);
            }
        }

        tempo += 0.1;

    }

    if (d) printLostDeadline();
    writeContext(out, changeContext);
    free(waiting);
    free(thread);
    free(mutex);
}

void insertProcess(pQueue* q, double t0, double dt, double deadline, char nome[], int l) {

    pr* new = malloc(sizeof(pr));
    new->t0 = t0;
    new->dt = dt;
    new->deadline = deadline;
    new->line = l;
    strcpy(new->nome, nome);

    q->fila[q->size++] = new;
}

void pushProcess(pList* L, pr* p) {

    pNode* node = malloc(sizeof(pNode));
    node->p = p;
    node->next = NULL;

    if (L->size) {
        L->fim->next = node;
        L->fim = node;
    }
    else L->ini = L->fim = node;

    L->size++;
}

pr* removeProcess(pList* L) {

    pNode *cur;
    pr* p;

    cur = L->ini;
    L->ini = cur->next;
    p = cur->p;
    L->size--;

    free(cur);

    return p;
}

pQueue* newProcessQueue() {

    pQueue* Q = malloc(sizeof(pQueue));
    Q->size = 0;

    return Q;
}

pList* newProcessList() {

    pList* L = malloc(sizeof(pList));
    L->ini = L->fim = NULL;
    L->size = 0;

    return L;
}

void assign(pQueue *q) {
    for (int i = 0; i < q->size; i++) {
        pr *temp = q->fila[i];
        temp->tid = i;
        q->fila[i] = temp;
    }
}

heap* newHeap() {
    heap *h = (heap *)malloc(sizeof(heap));
    if(!h) {
		printf("Error allocating memory...\n");
		exit(-1);
	}

	h->n = 0;
	h->size = 4;
	h->a = (double *) malloc(sizeof(double) * 4);
    h->pa = (pr **) malloc(sizeof(pr*) * 4);
	if(!h->a || !h->pa) {
		printf("Error allocating memory...\n");
		exit(-1);
	}

    return h;
}

void heapfy(heap *h, int loc) {
	int left, right, min;
	double temp;
    pr* ptemp;
	left = 2*(loc) + 1;
	right = left + 1;
	min = loc;


	if (left <= h->n && h->a[left] < h->a[min]) {
		min = left;
	}
	if (right <= h->n && h->a[right] < h->a[min]) {
		min = right;
	}

	if(min != loc) {
		temp = h->a[loc];
        ptemp = h->pa[loc];
		h->a[loc] = h->a[min];
        h->pa[loc] = h->pa[min];
		h->a[min] = temp;
        h->pa[min] = ptemp;
		heapfy(h, min);
	}

}

void insertHeap(heap *h, pr* p, int escal) {
	int index, parent;
    double value;
    if (escal) value = (double) p->dt;
    else value = (double) p->deadline;

	if (h->n == h->size - 1) {
		h->size *= 2;
		h->a = (double *)realloc(h->a, sizeof(double) * h->size);
        h->pa = (pr **)realloc(h->pa, sizeof(pr*) * h->size);
		if (!h->a || !h->pa) exit(-1);
	}

 	index = h->n++;

	for(;index; index = parent)
	{
		parent = (index - 1) / 2;
		if (h->a[parent] <= value) break;
		h->a[index] = h->a[parent];
        h->pa[index] = h->pa[parent];
	}
	h->a[index] = value;
    h->pa[index] = p;
}

pr* removeHeap(heap *h){
	double removed;
	double temp = h->a[--h->n];
    pr* premoved;
    pr* ptemp = h->pa[h->n];


	if ((h->n < (h->size/2)) && (h->size > 4)) {
		h->size /= 2;
		h->a = (double *)realloc(h->a, sizeof(double) * h->size);
        h->pa = (pr **)realloc(h->pa, sizeof(pr*) * h->size);
		if (!h->a || !h->pa) exit(-1); // Exit if the memory allocation fails
	}
 	removed = h->a[0];
    premoved = h->pa[0];
 	h->a[0] = temp;
    h->pa[0] = ptemp;
 	heapfy(h, 0);

 	return premoved;
}

int main(int argc, char const *argv[]) {

	FILE* in = fopen(argv[2], "r");
    out = fopen(argv[3], "w");

    if (argc == 5 && !strcmp(argv[4], "d")) d = 1;

    pQueue* Q = newProcessQueue();

    double t0, dt, deadline;
    char nome[MAXSIZE];

    int cont = 1;
    while (fscanf(in, "%lf %lf %lf %s", &t0, &dt, &deadline, nome) == 4) {
        insertProcess(Q, t0, dt, deadline, nome, cont++);
    }

    qsort(Q->fila, Q->size, sizeof(pr*), comp);
    assign(Q);

    int tipo = atoi(argv[1]);
	if 		(tipo == 1) shortestJobFirst(Q);
	else if (tipo == 2) roundRobin(Q);
	else if (tipo == 3) priority(Q);

	fclose(in);
	fclose(out);

	return 0;
}
