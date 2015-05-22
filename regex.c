#include <stdio.h>
#include <syslog.h>
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
    if( retex ){ syslog(LOG_ERR, "Could not compile regex\n"); exit(1); }
        while (regexec(&regex, rawPnt, 1, &pmatch, 0) == 0) {
            sprintf(res, "%.*s",  (int)(pmatch.rm_eo - pmatch.rm_so), &rawPnt[pmatch.rm_so]);
            stat->result[j] = atof(res);
            rawPnt += pmatch.rm_eo;
            j++;
        }
    regfree(&regex);

}
