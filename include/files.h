#pragma once
#include "flash.h"

#define FS_CAPACITY	(CAPACITY)
#define FS_MAXSIZE	(8*1024)
#define FS_MAXFILES	(FS_CAPACITY/FS_MAXSIZE)
#define FS_MAXNAME	15
#define FS_MAXBYTES	(FS_MAXSIZE-PAGE_LEN)


int format(char *name);
int remove(char *name);
int exist(char *name);
int rename(char *n1, char *n2);
int list(int *next, char *buff);
int load(char *name, char *buff);
int save(char *name, char *buff, int n);

