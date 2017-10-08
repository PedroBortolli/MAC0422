#include "ep1sh.h"

void shep1(char stack[][MAXSIZE]) {

    pid_t pid = fork();
    if (pid == 0) {
        char * const argv[] = {stack[0], stack[1], stack[2], stack[3], stack[4], NULL};
        execv("ep1", argv);
        exit(127);
    }
    else waitpid(pid, 0, 0);
}

void shPing(char stack[][MAXSIZE]) {
    if (!strcmp(stack[0], "/bin/ping")) {
        pid_t pid = fork();
        if (pid == 0) {
            char * const argv[] = {"ping", stack[1], stack[2], stack[3], NULL};
            execv(stack[0],argv);
            exit(127);
        }
        else waitpid(pid, 0, 0);
    }
}

void shChown(char stack[][MAXSIZE]) {
    if (!strcmp(stack[0], "chown")) {
        char *path, *user;
        path = getcwd(NULL, 1024);
        strcat(path, "/");
        strcat(path, stack[2]);
        struct group *grp;
        gid_t gid;

        grp = getgrnam(stack[1]);
        gid = grp->gr_gid;

        if (chown(path, -1, gid) == -1) {
            printf("Erro no chown do arquivo %s\n", path);
        }
        else printf("Arquivo %s tem o dono %s\n", path, stack[1]);
    }
}

void shCal(char stack[][MAXSIZE]) {
    if (!strcmp(stack[0], "/usr/bin/cal")) {
        pid_t pid = fork();
        if (pid == 0) {
            char * const argv[] = {"cal", stack[1], NULL};
            execv(stack[0],argv);
            exit(127);
        }
        else waitpid(pid, 0, 0);
    }
}

void shDate(char stack[][MAXSIZE]) {
    if (!strcmp(stack[0], "date")) {
        time_t t = time(NULL);
        struct tm *tm = localtime(&t);
        char s[64];
        strftime(s, sizeof(s), "%c", tm);
        printf("%s\n", s);
    }
    else if (!strcmp(stack[0], "exit")) exit(127);
}


int main(int argc, char const *argv[]) {
    char* in, shell[MAXSIZE];
    char stack[5][MAXSIZE];
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

        if (p == 5) shep1(stack);

        else if (p == 4) {

            if (!strcmp(stack[0], "./ep1")) shep1(stack);

            shPing(stack);
        }

        else if (p == 3) shChown(stack);

        else if (p == 2) shCal(stack);

        else if (p == 1) shDate(stack);

    }
    return 0;
}