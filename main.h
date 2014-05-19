#ifndef MAIN
#define MAIN

#define MAXCOUNT 30
#define OUTPUT_SIZE 1024
#define MAXSTAT 10

struct statStruct {
    int enabled;
    int type;
    const char *name;
    const char *cmmd;
    const char *regex;
    char raw[OUTPUT_SIZE]; 
    double  result[MAXSTAT];

};
typedef struct statStruct Status;

static int verbose_flag;
static int dry_flag;

char zeit[10];
const char *path;

void cmmdOutput(Status *stat);
void debug(Status *stat);
void timeSet();

#endif
