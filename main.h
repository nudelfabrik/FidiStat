#ifndef MAIN
#define MAIN

#define PATH    "~/"
#define CFG_FILE "config.cfg"

struct statStruct {
    int enabled;
    const char *name;
    const char *cmmd;
    const char *regex;
    char result[1024];

};

#endif
