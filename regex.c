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



void regexing(const char * reg, char* string) {
regex_t regex;
int retex;
size_t maxGroups = 3;
regmatch_t pmatch[maxGroups];
    retex = regcomp( &regex, reg, REG_EXTENDED);
    if( retex ){ fprintf(stderr, "Could not compile regex\n"); exit(1); }
    if (regexec(&regex, string, maxGroups, pmatch, 0) == 0) {
        char stringCp[strlen(string) + 1];
        strcpy(stringCp, string);
        stringCp[pmatch[0].rm_eo] = 0;
        strcpy(string, stringCp + pmatch[0].rm_so);
                
    }
    sanitize(string);
    
}

void sanitize(char* string) {
    int i;
    for (i = 0; i < strlen(string)+ 1  ; i++) {
        if (string[i] == ',') {
            string[i] = '.';
        }
    }
}
