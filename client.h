#ifndef CLIENT
#define CLIENT
#include "main.h"
#include <tls.h>

int verbose_flag;
int dry_flag;
int clean_flag, delete_flag, now_flag;

char zeit[10];
const char *path;
int maxCount;
int local;
const char *clientName;
const char *serverURL;
const char *serverPort;
struct tls_config* tlsClient_conf;

void client(void);

struct tls* initCon(connType type, int size);
void initTLS(void);
void deinitTLS(void);
void sendStat(Status *stat, int statNum);
void confSetup(Status stats[]);
void fixtime(void);
int processCommand(Status *stat);
void addMissingJson(void);
void timeSet();
void del(Status *stat);
void setLocation(char* loc);

#endif
