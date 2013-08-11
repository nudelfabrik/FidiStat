#ifndef MAIN
#define MAIN

#define PATH    "~/"
#define CFG_FILE "config.cfg"
#define MAXCOUNT 30
#define OUTPUT_SIZE 1024

struct statStruct {
    int enabled;
    const char *name;
    const char *cmmd;
    const char *regex;
    char raw[OUTPUT_SIZE] ;
    double  result[10];

};
char zeit[10];
void cmmdOutput(int i);

#endif
