// trata da alocacao de memoria

#include "aloc.h"

using namespace std;

mem info_aloc;
vector <process> proc_aloc;
set <int> often_requested_memory;
map <int, set <int> > available_pos;

void print_memory(vector <int> memory) {
	printf("\n");
	for (int i = 0; i < memory.size(); i++)
		printf("%d", memory[i]);
	printf("\n");
}

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
	printf("Need = %d\n", need);
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
	if (pos == -1) {
		return -1;
	}
	for (int i = pos; i < pos+need; i++) {
		memory[i] = proc.id;
	}
	print_memory(memory);
	printf("Pos = %d\n", pos);
	return pos;
}

int aloc_worst(vector <int> &memory, process proc) {
	int cont = 0, pos = -1, best = 0, //best = -infinito
	need = proc.b / info_aloc.aloc_size + (proc.b % info_aloc.aloc_size != 0);

	for (int i = 0; i < memory.size(); i++) {
		if (!memory[i]) cont++;
		else {
			if (cont > best && cont >= need) {
				best = cont;
				pos = i-cont;
			}
			cont = 0;
		}
	}
	if (cont > best && cont >= need) {
		pos = memory.size()-cont;
	}
	if (pos == -1) {
		printf("Sem espaco para alocar o processo %s\n", proc.name);
		return -1;
	}
	for (int i = pos; i < pos+need; i++) {
		memory[i] = proc.id;
	}
	return pos;
}

int aloc_quick(vector <int> &memory, process proc) {
	int need = need = proc.b / info_aloc.aloc_size + (proc.b % info_aloc.aloc_size != 0);
	if (often_requested_memory.find(proc.b) != often_requested_memory.end()) {
		if (available_pos[proc.b].empty()) return -1;
		else {
			int pos = *available_pos[proc.b].begin();
			for (int i = pos; i < pos+need; i++)
				memory[i] = 1;
			update_most_requested(memory);
			return pos;
		}
	}
	else {
		int cont = 0, pos = -1;
		for (int i = 0; i < memory.size(); i++) {
			if (!memory[i]) cont++;
			if (cont >= need) {
				pos = i-cont+1;
				break;
			}
			if (memory[i]) cont = 0;
		}
		if (pos == -1) return -1;
		else {
			for (int i = pos; i < pos+need; i++)
				memory[i] = 1;
			update_most_requested(memory);
			return pos;
		}
	}
}

void free_memory(vector <int> &memory, process proc) {
	printf("Preciso tirar a partir de %d\n", proc.aloc_pos);
	int need = proc.b / info_aloc.aloc_size + (proc.b % info_aloc.aloc_size != 0);
	for (int i = proc.aloc_pos; i < proc.aloc_pos + need; i++)
		memory[i] = 0;
	print_memory(memory);
}

void compact(vector <int> &memory, vector <process> &processos) {
	int cont = 0;
	for (int i = memory.size()-1; i >= 0; i--) {
		if (!memory[i]) {
			memory.erase(memory.begin() + i);
			memory.push_back(0);
		}
		else {
			processos[memory[i]-1].aloc_pos = i;
		}
	}
	print_memory(memory);
}

void get_most_requested(vector <process> processos, vector <int> memory) {
	set <pair <int, int> > s;
	set <int> ret;
	map <int, int> mp;
	for (int i = 0; i < processos.size(); i++) {
		int mem = processos[i].b;
		if (mp.find(mem) == mp.end()) {
			mp[mem] = 1;
			s.insert(make_pair(1, mem));
		}
		else {
			s.erase(s.find(make_pair(mp[mem], mem)));
			mp[mem]++;
			s.insert(make_pair(mp[mem], mem));
		}
	}
	int cont = 10;
	while (!s.empty() && cont) {
		cont--;
		set <pair <int, int> >::reverse_iterator it = s.rbegin();
		pair <int, int> p = *it;
		often_requested_memory.insert(p.second);
		s.erase(s.find(p));
	}
	/*
	for (set <int>::iterator it = often_requested_memory.begin(); it != often_requested_memory.end(); it++)
		printf("%d ", *it);
	printf("\n\n");
	*/
	update_most_requested(memory);
}

void update_most_requested(vector <int> memory) {
	for (set <int>::iterator it = often_requested_memory.begin(); it != often_requested_memory.end(); it++) {
		available_pos[*it].clear();
	}
	for (int i = 0; i < memory.size(); i++) {
		int cont = 0;
		for (int j = i; j < memory.size(); j++) {
			if (!memory[i]) cont++;
			else break;
		}
		for (set <int>::iterator it = often_requested_memory.begin(); it != often_requested_memory.end(); it++) {
			if (cont >= *it) available_pos[*it].insert(i);
		}
	}
}