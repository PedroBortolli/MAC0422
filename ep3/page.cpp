// trata da paginacao

#include "page.h"

using namespace std;

mem info_page;
vector <process> proc_page;

void page_init(vector <process> processos, mem info_init) {

    proc_page = processos;

    info_page.total     = info_init.total;
    info_page.virt      = info_init.virt;
    info_page.aloc_size = info_init.aloc_size;
    info_page.page_size = info_init.page_size;
}


void page_optimal() {


}

void page_fifo() {


}

void page_lru2() {


}

void page_lru4() {


}
