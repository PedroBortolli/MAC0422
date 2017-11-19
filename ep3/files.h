#ifndef FILES_H
#define FILES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include "info.h"

using namespace std;

void files_init(mem info_init);

void files_print_mem(int id, int offset);

void files_print_vir(vector <int> memory);

void files_close();

#endif
