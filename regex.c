/*
 * Written by Benedikt Scholtes
 * bene.scholtes@gmail.com
 * 2013
 */

#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include <string.h>
#include "regex.h"
#include "main.h"

struct statStruct stats[10];

void regexing(int i) {
    regex_t regex;
    int retex;
    regmatch_t pmatch;
    const char *rawPnt = stats[i].raw;

    retex = regcomp( &regex, stats[i].regex, REG_EXTENDED);
    if( retex ){ fprintf(stderr, "Could not compile regex\n"); exit(1); }
        while (regexec(&regex, rawPnt, 1, &pmatch, 0) == 0) {
            sprintf(stats[i].result, "%s%.*s\n",stats[i].result,  (int)(pmatch.rm_eo - pmatch.rm_so), &rawPnt[pmatch.rm_so]);
            rawPnt += pmatch.rm_eo;
        }
    sanitize(i);
}

void sanitize(int i) {
    int j;
    for (j = 0; j < strlen(stats[i].result)+ 1  ; j++) {
        if (stats[i].result[j] == ',') {
            stats[i].result[j] = '.';
        }
    }
}
