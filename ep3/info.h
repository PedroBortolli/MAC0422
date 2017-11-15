#ifndef INFO_H
#define INFO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

using namespace std;

typedef struct info {
	double t0, tf;
	int b, aloc_pos, id;
	char name[128];
	vector <int> p, t;
} process;

typedef struct memory_info {
    int total, virt, aloc_size, page_size;
} mem;

#endif
