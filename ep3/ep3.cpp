/*###########################################
##                                         ##
##  EP3 - MAC0422 - Sistemas Operacionais  ##
##  Pedro Vítor Bortolli Santos - 9793721  ##
##  Jonas Arilho Levy - 9344935            ## 
##                                         ##
###########################################*/


#include "ep3.h"
#include "aloc.h"
#include "page.h"
#include "simulate.h"

using namespace std;

mem info_init;
vector <process> processos_init;

void read(FILE * file) {
	double* first = (double*)malloc(8 * sizeof(double*));
	char* line = (char*)malloc(1024 * sizeof(char*));
	fscanf(file, " %[^\n]", line);

	char* split = strtok(line, " \t");
	int p = 0;
	while (split != NULL) {
		first[p++] = atof(split);
		split = strtok(NULL, " \t");
	}
    //informacoes iniciais
    info_init.total     = first[0];
    info_init.virt      = first[1];
    info_init.aloc_size = first[2];
    info_init.page_size = first[3];

	int aloc_size = 1;
	p = 1;
	while (fscanf(file, " %[^\n]", line) != EOF) {
		split = strtok(line, " \t");
		process process_line;
		int cont = 0;
		while (split != NULL) {
			if (!cont) process_line.t0 = atof(split);
			else if (cont == 1) {
				if (!strcmp(split, "COMPACTAR")) {
					strcpy(process_line.name, split);
					break;
				}
				else process_line.tf = atof(split);
			}
			else if (cont == 2) process_line.b = atof(split);
			else if (cont == 3) strcpy(process_line.name, split);
			else {
				if (cont%2 == 0) process_line.p.push_back(atof(split));
				else process_line.t.push_back(atof(split));
			}
			cont++;
			split = strtok(NULL, " \t");
		}
		process_line.id = p++;
		processos_init.push_back(process_line);
        process_line.p.clear();
        process_line.t.clear();
	}
	free(line);
	free(first);
}

void sim() {
	//pegar do prompt os dois numeros da funcao abaixo

    char* in, shell[100];
    char stack[3][100];

    FILE * arq = NULL;
    int aloc = 0, page = 0;

    while (1) {
        snprintf(shell, sizeof(shell), "[%s] $ ", getcwd(NULL, 1024));
        in = readline(shell);
        if (!in) break;
        add_history(in);
        char* split = strtok(in, " \t");
        int p = 0;
        while (split != NULL) {
            strcpy(stack[p++], split);
            split = strtok(NULL, " \t");
        }

        free(in);

        if (p == 2) {

            if (!strcmp(stack[0], "carrega")) {
                arq = fopen(stack[1], "r");
                if (!arq) printf("Nao foi possivel ler este arquivo\n");
                else {
                	read(arq);
                	printf("Arquivo '%s' carregado\n", stack[1]);
                }
            }
            else if (!strcmp(stack[0], "espaco")) {
            	aloc = atoi(stack[1]);
            	if (aloc <= 0 || aloc >= 4) printf("Comando invalido\n");
            	else printf("Espaco selecionado: %d\n", aloc);
            }

            else if (!strcmp(stack[0], "substitui")) {
            	page = atoi(stack[1]);
            	if (page <= 0 || page >= 5) printf("Comando invalido\n");
            	else printf("Pagina selecionada: %d\n", page);
            }

            else if (!strcmp(stack[0], "executa")) {
                if(!arq) printf("Favor carregar um arquivo antes de executar!\n");
                else if(aloc <= 0 || aloc >= 4) printf("Favor inserir um espaco antes de executar!\n");
                else if(page <= 0 || page >= 5) printf("Favor inserir um tamanho de pagina antes de executar!\n");
                else simulate(processos_init, aloc, page, info_init, atof(stack[1]));
            }
            else printf("Comando invalido\n");
        }

        else if (p == 1) {
            if (!strcmp(stack[0], "sai")) {
                exit(127);
                fclose(arq);
            }
            else printf("Comando invalido\n");
        }

        else printf("Comando invalido\n");

    }
}


void print() {
	//printf("\n%d %d %d %d\n\n", total, virt, aloc_size, page_size);
	for (int i = 0; i < (int)processos_init.size(); i++) {
		printf("%s :\n", processos_init[i].name);
		printf("%.1lf %.1lf %d\n", processos_init[i].t0, processos_init[i].tf, processos_init[i].b);
		printf("	");
		for (int j = 0; j < processos_init[i].p.size(); j++) {
			printf("%lf %lf ", processos_init[i].p[j], processos_init[i].t[j]);
		}
		printf("\n\n");
	}
}


int main(int argc, const char* argv[]) {
	printf(" \n");
	printf("\n");

    sim();

	print();

	return 0;
}
