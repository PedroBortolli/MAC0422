/*###########################################
##                                         ##
##  EP3 - MAC0422 - Sistemas Operacionais  ##
##  Pedro Vítor Bortolli Santos - 9793721  ##
##  Jonas Arilho Levy - 9344935            ## 
##                                         ##
###########################################*/


// trata da paginacao

#include "page.h"
#include "files.h"

using namespace std;

mem info_page;
int n_frames, n_page_fault;
vector <process> proc_page;
vector <vir_page> page_table;
vector <frame> frame_table;
vector <process_pages> pages_of;
//2
queue <int> fifo_page;
//3
bool **mat;
//4
vector < list <bool> > counter;

void page_init(vector <process> processos, mem info_init, int mode) {

    proc_page = processos;

    info_page.total     = info_init.total;
    info_page.virt      = info_init.virt;
    info_page.aloc_size = info_init.aloc_size;
    info_page.page_size = info_init.page_size;

    n_frames = (info_page.total / info_page.page_size) + (info_page.total % info_page.page_size != 0);
    n_page_fault = 0;

    switch(mode) {
        case 1:
            //optimal
            break;
        case 2:
            //fifo
            break;
        case 3:
            //inicializa matriz
            mat = (bool**)malloc(n_frames * sizeof(bool*));
            for (int i = 0; i < n_frames; i++) {
                mat[i] = (bool*)malloc(n_frames * sizeof(bool));
            }
            for (int i = 0; i < n_frames; i++) {
                for (int j = 0; j < n_frames; j++) {
                    mat[i][j] = 0;
                }
            }
            break;
        case 4:
            //inicializa counter
            for (int i = 0; i < n_frames; i++) {
                list <bool> l;
                for (int j = 0; j < 8; j++) {
                    l.push_back(0);
                }
                counter.push_back(l);
            }
            break;
    }

    //cria frames
    for (int i = 0; i < n_frames; i++) {
        frame f;
        f.r = 0;
        f.empty = 1;
        frame_table.push_back(f);
    }

}


void page_optimal(int pos, process proc) {

    page_fifo(pos, proc);

}

void page_fifo(int pos, process proc) {

    int proc_page_id, target_page_id, target_frame_id;

    if (page_unpaged_process(proc.id)) {
        page_create(proc, pos);
        pos = (pos - page_table[pages_of[proc.id].page_id[0]].base);
    }

    proc_page_id = pos / info_page.page_size;
    target_page_id = pages_of[proc.id].page_id[proc_page_id];

    if (!page_table[target_page_id].loaded) {
        n_page_fault++;
        if(fifo_page.size() >= n_frames) {
            int page_to_suspend = fifo_page.front();
            fifo_page.pop();
            page_suspend(page_to_suspend, page_table[page_to_suspend].frame_id);
            target_frame_id = page_table[page_to_suspend].frame_id;
            // print -1 na memoria fisica
            for (int ini = target_frame_id * info_page.page_size;
                ini <= (target_frame_id + 1)* info_page.page_size;
                ini += info_page.aloc_size) {
                files_print_mem(-1, ini);
            }
        } else {
            for (int i = 0; i < frame_table.size(); i++) {
                if(frame_table[i].empty) {
                    target_frame_id = i;
                    break;
                }
            }
        }
        page_load(target_page_id, target_frame_id);
        // print i na memoria fisica
        for (int ini = target_frame_id * info_page.page_size;
            ini <= (target_frame_id + 1) * info_page.page_size;
            ini += info_page.aloc_size) {
            files_print_mem(page_table[target_page_id].pid, ini);
        }
        fifo_page.push(target_page_id);
    }
}

void page_lru2(int pos, process proc) {

    int proc_page_id, target_page_id, target_frame_id;

    if (page_unpaged_process(proc.id)) {
        page_create(proc, pos);
        pos = (pos - page_table[pages_of[proc.id].page_id[0]].base);
    }

    proc_page_id = pos / info_page.page_size;
    target_page_id = pages_of[proc.id].page_id[proc_page_id];

    if (!page_table[target_page_id].loaded) {
        n_page_fault++;
        int i;
        for (i = 0; i < n_frames; i++) {
            int count = 0;
            for (int j = 0; j < n_frames; j++) {
                count = count + (int)mat[i][j];
            }
            if (!count) {
                if(!frame_table[i].empty)
                    page_suspend(frame_table[i].page_id, i);
                break;
            }
        }
        page_load(target_page_id, i);
        // print i na memoria fisica
        int allocated_size = 0;
        while (allocated_size < proc.b)
            allocated_size += info_page.aloc_size;
        for (int ini = i * info_page.page_size;
            ini <= allocated_size;
            ini += info_page.aloc_size) {
            files_print_mem(page_table[target_page_id].pid, ini);
        }
    }

    target_frame_id = page_table[target_page_id].frame_id;
    for (int i = 0; i < n_frames; i++) {
        mat[target_frame_id][i] = 1;
    }
    for (int i = 0; i < n_frames; i++) {
        mat[i][target_frame_id] = 0;
    }

}

void page_lru4(int pos, process proc) {

    int proc_page_id, target_page_id, target_frame_id;

    if (page_unpaged_process(proc.id)) {
        page_create(proc, pos);
        pos = (pos - page_table[pages_of[proc.id].page_id[0]].base);
    }

    proc_page_id = pos / info_page.page_size;
    target_page_id = pages_of[proc.id].page_id[proc_page_id];

    if (!page_table[target_page_id].loaded) {
        n_page_fault++;
        int min = 11111111, min_frame;
        for (int i = 0; i < n_frames; i++) {
            int count = 0;
            for (std::list<bool>::iterator it = counter.at(i).begin();
            it != counter.at(i).end(); ++it) {
                count += (int) *it;
            }
            if (count < min) {
                min = count;
                min_frame = i;
            }
        }
        if(!frame_table[min_frame].empty)
            page_suspend(frame_table[min_frame].page_id, min_frame);

        page_load(target_page_id, min_frame);
        // print i na memoria fisica
        int allocated_size = 0;
        while (allocated_size < proc.b)
            allocated_size += info_page.aloc_size;
        for (int ini = min_frame * info_page.page_size;
            ini <= allocated_size;
            ini += info_page.aloc_size) {
            files_print_mem(page_table[target_page_id].pid, ini);
        }
    }
    target_frame_id = page_table[target_page_id].frame_id;
    for (int i = 0; i < n_frames; i++) {
        counter.at(i).pop_back();
        counter.at(i).push_front(frame_table[i].r);
    }

}

int page_unpaged_process(int pid) {

    if (pid >= pages_of.size()) return 1;
    else if (pages_of[pid].pid == pid) return 0;

    return 1;
}

void page_create(process proc, int pos) {
    if (proc.id >= pages_of.size()) pages_of.resize(proc.id + 1);

    process_pages new_pr;
    new_pr.pid = proc.id;

    int allocated_size = 0, n_pages;
    while (allocated_size < proc.b)
        allocated_size += info_page.aloc_size;

    n_pages = allocated_size / info_page.page_size + (allocated_size % info_page.page_size != 0);
    for (int i = 0; i < n_pages; i++) {
        new_pr.page_id.push_back(page_table.size());
        vir_page new_page;
        new_page.id = page_table.size();
        new_page.loaded = 0;
        new_page.pid = proc.id;
        new_page.base = pos;
        pos += info_page.page_size;
        new_page.size = new_page.base + info_page.page_size;
        page_table.push_back(new_page);
    }

    pages_of.insert(pages_of.begin() + proc.id, new_pr);
}

void page_load(int id_page, int id_frame) {
    frame_table[id_frame].r = 1;
    frame_table[id_frame].empty = 0;
    frame_table[id_frame].page_id = id_page;

    page_table[id_page].loaded = 1;
    page_table[id_page].frame_id = id_frame;
}

void page_suspend(int id_page, int id_frame) {
    frame_table[id_frame].r = 0;
    frame_table[id_frame].empty = 1;

    page_table[id_page].loaded = 0;
}

void page_reset_r() {

    for (int i = 0; i < n_frames; i++) {
        frame_table[i].r = 0;
    }
}

void page_end_process(process proc) {

    int n_of_pages = pages_of[proc.id].page_id.size();
    for (int i = 0; i < n_of_pages; i++) {
        int this_page_id = pages_of[proc.id].page_id.at(i);
        if(page_table[this_page_id].loaded) {
            int this_frame_id = page_table[this_page_id].frame_id;
            page_suspend(this_page_id, this_frame_id);
            // print -1 na memoria fisica
            for (int ini = this_frame_id * info_page.page_size;
                ini <= (this_frame_id + 1)* info_page.page_size;
                ini += info_page.aloc_size) {
                files_print_mem(-1, ini);
            }
        }
    }
}

int page_fault_number() {

    return n_page_fault;
}

void page_exit() {

    //free em tudo

}
