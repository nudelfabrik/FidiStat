#ifndef MAIN
#define MAIN

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <getopt.h>
#include <errno.h>
#include <libutil.h>
#include <signal.h>
#include <syslog.h>
#include <tls.h>

#define OUTPUT_SIZE 1024
#define MAXSTAT 10

struct statStruct {
    int enabled;
    int type;
    const char *name;
    const char *cmmd;
    const char *csv;
    float result[MAXSTAT];
};

enum connectionEnum {
    HELLO,
    UPDATE,
    CREATE,
    DELETE
};

extern volatile sig_atomic_t term;
void handleSigterm(int sig);
typedef enum connectionEnum connType;
typedef struct statStruct Status;
void client_start();
void client_stop();
void client_restart();

void server_start();
void server_stop();
void server_restart();

void debug(Status *stat);
void handleFlags(int argc, const char *argv[]);
char* composeFileName(const char* prefix, const char* name, const char* type);

#endif
