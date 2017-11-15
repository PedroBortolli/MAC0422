// trata da alocacao de memoria

#include "aloc.h"

using namespace std;

mem info_aloc;
vector <process> proc_aloc;

void aloc_init(vector <process> processos, mem info_init) {

    proc_aloc = processos;

    info_aloc.total     = info_init.total;
    info_aloc.virt      = info_init.virt;
    info_aloc.aloc_size = info_init.aloc_size;
    info_aloc.page_size = info_init.page_size;
}

//finds the shortest available piece of memory to allocate process
int aloc_best(vector <int> &memory, process proc) {
	int cont = 0, pos = -1, best = 9999999, //best = infinito
	need = proc.b / info_aloc.aloc_size + (proc.b % info_aloc.aloc_size != 0);
	printf("Need = %d  pro  processo %s\n", need, proc.name);

	for (int i = 0; i < memory.size(); i++) {
		if (!memory[i]) cont++;
		else {
			if (cont < best && cont >= need) {
				best = cont;
				pos = i-cont;
			}
			cont = 0;
		}
	}
	if (cont < best && cont >= need) {
		pos = memory.size()-cont;
	}
	printf("	pos = %d\n", pos);
	if (pos == -1) {
		printf("Sem espaco para alocar o processo %s\n", proc.name);
		return -1;
	}
	for (int i = pos; i < pos+need; i++) {
		memory[i] = 1;
	}
	return pos;
}

void aloc_worst(vector <int> &memory, process proc) {


}

void aloc_quick(vector <int> &memory, process proc) {


}

void free_memory(vector <int> &memory, process proc) {
	printf("Preciso tirar a partir de %d\n", proc.aloc_pos);
	int need = proc.b / info_aloc.aloc_size + (proc.b % info_aloc.aloc_size != 0);
	for (int i = proc.aloc_pos; i < proc.aloc_pos + need; i++)
		memory[i] = 0;
	printf("\n");
	for (int i = 0; i < memory.size(); i++)
		printf("%d", memory[i]);
	printf("\n");
}
