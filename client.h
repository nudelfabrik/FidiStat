#ifndef CLIENT
#define CLIENT
#include "main.h"
#include <tls.h>

int verbose_flag, delete_flag, now_flag;

char zeit[10];
struct tls_config* tlsClient_conf;

void client(commandType type);
void confSetup(Status stats[]);
int processCommand(Status *stat);

// TLS functions
void sendHello(Status stat[]);
struct tls* initCon(connType type, int size);
void initTLS(void);
void deinitTLS(void);
void sendStat(Status *stat, int statNum);

// time functions
void fixtime(void);
void timeSet();

#endif
