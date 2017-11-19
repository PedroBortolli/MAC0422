#include "simulate.h"
#include "info.h"
#include "aloc.h"
#include "page.h"
#include "files.h"


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
	vector <int> memory(info.virt / info.aloc_size);
	fill(memory.begin(), memory.end(), 0);
	set <pair <double, int> > final;

    aloc_init(processos, info);
    page_init(processos, info);
    files_init(info);

	int i = 0, pos;
	clock_t begin = clock();

	if (aloc == 3) get_most_requested(processos, memory);

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
							printf("Pos retornada = %d\n", pos);
							break;
						case 2:
							pos = aloc_worst(memory, p);
							printf("Pos retoxssxqsqxsrnada = %d\n", pos);
							break;
						case 3:
							pos = aloc_quick(memory, p);
							printf("possss = %d\n", pos);
							break;
					}
					switch(page) {
						case 1:
							page_optimal(pos, p);
							break;
						case 2:
							page_fifo(pos, p);
							break;
						case 3:
							page_lru2(pos, p);
							break;
						case 4:
							page_lru4(pos, p);
							break;
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
				printf("Tirando %.1lf\n", t);
				final.erase(final.begin());
				free_memory(memory, processos[p.second]);
				printf("	aight\n");
			}
		}
		else if (fim) break;
	}

    page_exit();
    files_close();
}
