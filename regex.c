/*
 * Written by Benedikt Scholtes
 * bene.scholtes@gmail.com
 * 2013
 */

#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include <string.h>
#include "main.h"
#include "regex.h"

struct statStruct stats[10];

void regexing(int i) {
    regex_t regex;
    int retex;
    regmatch_t pmatch;
    const char *rawPnt = stats[i].raw;
    char res[OUTPUT_SIZE];
    int j = 0;

    retex = regcomp( &regex, stats[i].regex, REG_EXTENDED);
    if( retex ){ fprintf(stderr, "Could not compile regex\n"); exit(1); }
        while (regexec(&regex, rawPnt, 1, &pmatch, 0) == 0) {
            sprintf(res, "%.*s",  (int)(pmatch.rm_eo - pmatch.rm_so), &rawPnt[pmatch.rm_so]);
            sanitize(res);
            stats[i].result[j] = atof(res);
            rawPnt += pmatch.rm_eo;
            j++;
        }
}

void sanitize(char c[OUTPUT_SIZE]) {
    int j;
    int k;
    for (k = 0; k < 10; k++) {
        
    for (j = 0; j < strlen(c)- 1  ; j++) {
        if (c[j] == ',') {
            c[j] = '.';
        }
    }
        }
}
