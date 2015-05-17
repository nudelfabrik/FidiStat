#ifndef CLIENT
#define CLIENT
#include "main.h"

int verbose_flag;
int dry_flag;
int clean_flag, delete_flag;

char *cfgLocation = "/usr/local/etc/fidistat/config.cfg";
char zeit[10];
const char *path;
int maxCount;

void confSetup(Status stats[]);
void fixtime(void);
int cmmdOutput(Status *stat);
void addMissingJson(void);
void timeSet();
void del(Status *stat);

#endif
