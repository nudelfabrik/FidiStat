#ifndef MAIN
#define MAIN

#define PATH    "~/"
#define CFG_FILE "config.cfg"
#define OUTPUT_SIZE 1024

struct statStruct {
    int enabled;
    const char *name;
    const char *cmmd;
    const char *regex;
    char raw[OUTPUT_SIZE] ;
    char  result[OUTPUT_SIZE];

};

void cmmdOutput(int i);

#endif
