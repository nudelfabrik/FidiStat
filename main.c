/*
 * Written by Benedikt Scholtes
 * bene.scholtes@gmail.com
 * 2013
 */

#include <jansson.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include <libconfig.h>
#include "regex.h"
#include "config.h"
#include "main.h"

struct statStruct stats[10];

int main(int argc, const char *argv[])
{
    json_t *root;
    json_error_t error;
    
    initConf();
    int statNum = getStatNum(); 
 
    //import the config file
    int i = 0;
    for (i = 0; i < statNum; i++) {
        getConfList(i);
        getConfEnable(i);
        if (stats[i].enabled) {
            getConfRegex(i);
            getConfCmmd(i);
            cmmdOutput(i); //Speicher Cmmd Output
            regexing(i);
        }
    }

    destroyConf(); 
    return 0;
}

void cmmdOutput(int i) {
    char raw[OUTPUT_SIZE];
    FILE *fp;

    fp = popen(stats[i].cmmd, "r");  
    while(fgets(raw, sizeof(stats[i].raw-1), fp) != NULL) {
        strcat(stats[i].raw,raw);
    }
    pclose(fp);
}

