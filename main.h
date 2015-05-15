#ifndef MAIN
#define MAIN

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

int verbose_flag;
int dry_flag;
int clean_flag, delete_flag;

char zeit[10];
const char *path;
int maxCount;

void confSetup(Status stats[]);
void fixtime(void);
int cmmdOutput(Status *stat);
void addMissingJson(void);
void debug(Status *stat);
void timeSet();
void del(Status *stat);
void handleFlags(int argc, const char *argv[]);

#endif
