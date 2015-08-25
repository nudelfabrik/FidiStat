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
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <syslog.h>
#include <tls.h>

#define OUTPUT_SIZE 1024
int verbose_flag, delete_flag, now_flag;

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
};

typedef struct statStruct Status;
typedef enum connectionEnum connType;
typedef enum commandEnum commandType;

extern volatile sig_atomic_t term;

struct pidfh* daemon_start(char who);
void daemon_stop(char who);

void handleFlags(int argc, const char *argv[]);
char* composeFileName(const char* prefix, const char* name, const char* type);
void delete(const char *client, const char *name);
void debug(Status *stat);

#endif
