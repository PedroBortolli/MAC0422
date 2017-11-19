#ifndef ALOC_H
#define ALOC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <set>
#include <map>
#include "info.h"

using namespace std;

void print_memory(vector <int> memory);

void aloc_init(vector <process> processos, mem info_init);

int aloc_best(vector <int> &memory, process proc);

int aloc_worst(vector <int> &memory, process proc);

int aloc_quick(vector <int> &memory, process proc);

void free_memory(vector <int> &memory, process proc);

void compact(vector <int> &memory, vector <process> &processos);

void get_most_requested(vector <process> processos, vector <int> memory);

void update_most_requested(vector <int> memory);

#endif