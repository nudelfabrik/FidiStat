#ifndef MAIN
#define MAIN

#define OUTPUT_SIZE 1024
#define MAXSTAT 10

struct statStruct {
    int enabled;
    int type;
    const char *name;
    const char *ident;
    const char *cmmd;
    const char *csv;
    float result[MAXSTAT];
};

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
