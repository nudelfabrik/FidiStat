#ifndef MAIN
#define MAIN

#include <signal.h>

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

#endif
