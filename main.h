#ifndef MAIN
#define MAIN

#define PATH    "~/"
#define CFG_FILE "config.cfg"
#define MAXCOUNT 30
#define OUTPUT_SIZE 1024
#define MAXSTAT 10

struct statStruct {
    int enabled;
    const char *name;
    const char *cmmd;
    const char *regex;
    char raw[OUTPUT_SIZE];
    double  result[MAXSTAT];

};
typedef struct statStruct Status;

char zeit[10];
char path[OUTPUT_SIZE];
void cmmdOutput(Status *stat);
void timeSet();

#endif
