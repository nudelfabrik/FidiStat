#ifndef CLIENT
#define CLIENT
#include "main.h"

int verbose_flag;
int dry_flag;
int clean_flag, delete_flag, now_flag;

char zeit[10];
const char *path;
int maxCount;
int local;

void client(void);

void sendStat(Status *stat);
void confSetup(Status stats[]);
void fixtime(void);
int processCommand(Status *stat);
void addMissingJson(void);
void timeSet();
void del(Status *stat);
void setLocation(char* loc);

#endif
