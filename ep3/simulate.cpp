#include "simulate.h"
#include "info.h"
#include "aloc.h"
#include "page.h"


/*  aloc
	1 - best
	2 - worst
	3 - quick

	page
	1 - optimal
	2 - fifo
	3 - lru2
	4 - lru4
*/

void simulate(vector <process> processos, int aloc, int page, mem info) {
	vector <int> memory(20);
	fill(memory.begin(), memory.end(), 0);
	set <pair <double, int> > final;

    aloc_init(processos, info);
    page_init(processos, info);

	int i = 0, pos;
	clock_t begin = clock();
	while (1) {
		clock_t now = clock();
		double elapsed = double(now - begin) / CLOCKS_PER_SEC;
		int fim = 0;
		if (i < processos.size()) {
			process p = processos[i];
			if (fabs((double)p.t0 - elapsed) < 0.001) {
				
				if (!strcmp(p.name, "COMPACTAR")) {
					printf("Chamando compactar\n");
					compact(memory, processos);
					printf("	naisu\n");
				}

				else {
					switch(aloc) {
						case 1:
							pos = aloc_best(memory, p);
						case 2:
							pos = aloc_worst(memory, p);
						case 3:
							pos = aloc_quick(memory, p);
					}
					switch(page) {
						case 1:
							page_optimal();
						case 2:
							page_fifo();
						case 3:
							page_lru2();
						case 4:
							page_lru4();
					}
					processos[i].aloc_pos = pos;
					final.insert(make_pair(p.tf, i));
				}
				i++;
			}
		}
		else fim = 1;
		if (!final.empty()) {
			pair <double, int> p = *(final.begin());
			double t = (double)p.first;
			if (fabs(t-elapsed) < 0.001) {
				//remove esse processo pq ele ja acabou
				final.erase(final.begin());
				free_memory(memory, processos[p.second]);
			}
		}
		else if (fim) break;
	}
}
