#ifndef PAGE_H
#define PAGE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <ctime>
#include "info.h"

using namespace std;

void page_init(vector <process> processos, mem info_init);

void page_optimal();

void page_fifo();

void page_lru2();

void page_lru4();

#endif
