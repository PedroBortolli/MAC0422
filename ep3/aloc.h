#ifndef ALOC_H
#define ALOC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include "info.h"

using namespace std;

void aloc_init(vector <process> processos, mem info_init);

int aloc_best(vector <int> &memory, process proc);

void aloc_worst(vector <int> &memory, process proc);

void aloc_quick(vector <int> &memory, process proc);

void free_memory(vector <int> &memory, process proc);

#endif
