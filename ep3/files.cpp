/*###########################################
##                                         ##
##  EP3 - MAC0422 - Sistemas Operacionais  ##
##  Pedro Vítor Bortolli Santos - 9793721  ##
##  Jonas Arilho Levy - 9344935            ## 
##                                         ##
###########################################*/


#include "files.h"

using namespace std;

FILE * file_mem, * file_vir;
int sz, vir_sz;
char tmp_mem[15], tmp_vir[15];

void files_init(mem info_init) {

    //cria arquivo de memoria
    sz = info_init.aloc_size;
    vir_sz = info_init.virt;
    //printf("sz = %d\n", sz);

    memset(tmp_mem,0,sizeof(tmp_mem));
    memset(tmp_vir,0,sizeof(tmp_vir));

    strncpy(tmp_mem,"/tmp/ep3.mem", 12);
    strncpy(tmp_vir,"/tmp/ep3.vir", 12);
    char array_mem[info_init.total], array_vir[info_init.virt];
    for (int i = 0; i < info_init.total; i++) {
        array_mem[i] = -1;
    }
    for (int i = 0; i < info_init.virt; i++) {
        array_vir[i] = -1;
    }

    file_mem = fopen(tmp_mem, "wb+");
    file_vir = fopen(tmp_vir, "wb+");

    fwrite (array_mem, sizeof(char), info_init.total, file_mem);
    fwrite (array_vir, sizeof(char), info_init.virt, file_vir);
}

void files_print_bitmap(vector <int> memory) {
    printf("Estado do bitmap:\n");
    for (int i = 0; i < memory.size(); i++) {
        if (memory[i]) printf("1");
        else printf("0");
    }
    printf("\n");
}

void files_print_mem(int id, int offset) {

    char temp_array[sz];
    for (int i = 0; i < sz; i++) {
        temp_array[i] = (char)id;
    }

    fseek(file_mem, offset, SEEK_SET);
    fwrite (temp_array, sizeof(char), sz, file_mem);
}

void files_print_vir(vector <int> memory, int offset) {

    char temp_array[vir_sz];
    int p = 0;
    for (int i = 0; i < memory.size(); i++) {
        for (int j = 0; j < sz; j++) {
            temp_array[p++] = (char)(memory[i]-1);
        }
    }

    fseek(file_vir, offset*sz, SEEK_SET);
    fwrite(temp_array, sizeof(char), vir_sz, file_vir);
}

void files_close() {

    fclose(file_mem);
    fclose(file_vir);
    //remove os arquivos temporarios
    remove(tmp_mem);
    remove(tmp_vir);
}
