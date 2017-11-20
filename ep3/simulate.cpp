/*###########################################
##                                         ##
##  EP3 - MAC0422 - Sistemas Operacionais  ##
##  Pedro Vítor Bortolli Santos - 9793721  ##
##  Jonas Arilho Levy - 9344935            ## 
##                                         ##
###########################################*/


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

void simulate(vector <process> processos, int aloc, int page, mem info, double debug) {
	vector <int> memory(info.virt / info.aloc_size);
	fill(memory.begin(), memory.end(), 0);
	set <pair <double, int> > final;
	priority_queue <pair <pair <double, double>, process > > ativos;

    aloc_init(processos, info);
    page_init(processos, info, page);
    files_init(info);

	int i = 0, pos;
	double debug_time = debug;

	if (aloc == 3) get_most_requested(processos, memory);
	double fake_time = 0.0;
	while (1) {
		int fim = 0, ok = 0;
		if (i < processos.size()) {
			process pr = processos[i];
			if (fabs(pr.t0-fake_time) < 0.001) {
				for (int j = 0; j < pr.p.size(); j++) {
					ativos.push(make_pair(make_pair(-pr.t[j], pr.p[j]), pr));
				}
				if (!strcmp(pr.name, "COMPACTAR")) {
					compact(memory, processos);
				}

				else {
					switch(aloc) {
						case 1:
							pos = aloc_best(memory, pr);
							//printf("Pos retornada (1) = %d\n", pos);
							break;
						case 2:
							pos = aloc_worst(memory, pr);
							//printf("Pos retornada (2) = %d\n", pos);
							break;
						case 3:
							pos = aloc_quick(memory, pr);
							//printf("Pos retornada (3) = %d\n", pos);
							break;
					}
					switch(page) {
						case 1:
							page_optimal(pos, pr);
							break;
						case 2:
							page_fifo(pos, pr);
							break;
						case 3:
							page_lru2(pos, pr);
							break;
						case 4:
							page_lru4(pos, pr);
							break;
					}
					processos[i].aloc_pos = pos;
					final.insert(make_pair(pr.tf, i));
				}
				i++;
				fake_time -= 0.1;
				ok = 1;
			}
		}
		else fim = 1;

		if (!ativos.empty()) {
			pair <pair <double, double>, process > pr;
			pr = ativos.top();
			double tt = -pr.first.first;
			double pp = pr.first.second;
			process proc = pr.second;
			//faz as coisas aqui
            switch(page) {
                case 1:
                    page_optimal(pp, proc);
                    break;
                case 2:
                    page_fifo(pp, proc);
                    break;
                case 3:
                    page_lru2(pp, proc);
                    break;
                case 4:
                    page_lru4(pp, proc);
                    break;
            }


			if (!ok) {
				fake_time -= 0.1;
				ok = 1;
			}
			ativos.pop();
		}

		if (!final.empty()) {
			pair <double, int> pr = *(final.begin());
			double t = (double)pr.first;
			if (fabs(t-fake_time) < 0.001) {
				//remove esse processo pq ele ja acabou
				final.erase(final.begin());
                page_end_process(processos[pr.second]);
				free_memory(memory, processos[pr.second]);
				if (!ok) {
					fake_time -= 0.1;
					ok = 1;
				}
			}
			fim = 0;
		}
		if (fabs(debug_time-fake_time) < 0.001) {
			//printf("Debugando no tempo   %.1lfs\n", fake_time);
			files_print_bitmap(memory);
			debug_time += debug;
		}
        if (((int)(fake_time * 10) % 10) == 0) page_reset_r();
		else if (fim) break;
		fake_time += 0.1;
	}

    printf("num de page faults = %d\n", page_fault_number());
    page_exit();
    files_close();
}
