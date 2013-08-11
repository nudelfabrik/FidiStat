#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include <string.h>
#include "main.h"
#include "regex.h"


void regexing(Status *stat) {
    regex_t regex;
    int retex;
    regmatch_t pmatch;
    const char *rawPnt = stat->raw;
    char res[OUTPUT_SIZE];
    int j = 0;

    retex = regcomp( &regex, stat->regex, REG_EXTENDED);
    if( retex ){ fprintf(stderr, "Could not compile regex\n"); exit(1); }
        while (regexec(&regex, rawPnt, 1, &pmatch, 0) == 0) {
            sprintf(res, "%.*s",  (int)(pmatch.rm_eo - pmatch.rm_so), &rawPnt[pmatch.rm_so]);
            sanitize(res);
            stat->result[j] = atof(res);
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
