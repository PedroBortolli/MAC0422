#ifndef PAGE_H
#define PAGE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>
#include <queue>
#include <list>
#include <ctime>
#include "info.h"

using namespace std;


typedef struct page {
    int id, loaded, frame_id, pid, base, size;
} vir_page;

typedef struct frame {
    int r, empty, page_id;
} frame;

typedef struct process_pages {
    int pid;
    vector <int> page_id;
} process_pages;


void page_init(vector <process> processos, mem info_init, int mode);

void page_optimal(int pos, process proc);

void page_fifo(int pos, process proc);

void page_lru2(int pos, process proc);

void page_lru4(int pos, process proc);

int page_unpaged_process(int pid);

void page_create(process proc, int pos);

void page_load(int id_page, int id_frame);

void page_suspend(int id_page, int id_frame);

void page_reset_r();

void page_end_process(process proc);

int page_fault_number();

void page_exit();

#endif
