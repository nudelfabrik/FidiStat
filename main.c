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
#include "jansson.h"
#include <time.h>

struct statStruct stats[10];
char zeit[10];

int main(int argc, const char *argv[])
{
    initConf();
    int statNum = getStatNum(); 
    time_t epoch_time;
    struct tm *tm_p;
    epoch_time = time( NULL );
    tm_p = localtime( &epoch_time );
    sprintf(zeit, "%.2d:%.2d", 
    tm_p->tm_hour, tm_p->tm_min );
    //
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
            makeJansson(i);
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

