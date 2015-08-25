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

struct statStruct {
    int id;
    int enabled;
    int type;
    int num;
    const char *name;
    const char *cmmd;
    const char *csv;
    float *result;
};

enum connectionEnum {
    HELLO,
    NEWDATA,
    CREATE,
    UPDATE,
    DELETE
};

enum commandEnum {
    START,
    UPDT
}
char *cfgLocation = "/usr/local/etc/fidistat/config.cfg";

typedef struct statStruct Status;
typedef enum connectionEnum connType;
typedef enum commandEnum commandType;

extern volatile sig_atomic_t term;
void handleSigterm(int sig);

struct *pidfh daemon_start(char who);
void daemon_stop(char who);

void debug(Status *stat);
void handleFlags(int argc, const char *argv[]);
char* composeFileName(const char* prefix, const char* name, const char* type);

#endif
