#ifndef EP1SH_H
#define EP1SH_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>

#define  MAXSIZE 100

void shep1(char stack[][MAXSIZE]);

void shPing(char stack[][MAXSIZE]);

void shChown(char stack[][MAXSIZE]);

void shCal(char stack[][MAXSIZE]);

void shDate(char stack[][MAXSIZE]);

#endif