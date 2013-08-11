/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2013 Benedikt Scholtes
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "main.h"
#include "regex.h"
#include "config.h"
#include "jansson.h"

char path[OUTPUT_SIZE];
char zeit[10];

int main(int argc, const char *argv[])
{
    Status stats[10];
    Status *statsPtr;

    //load Config File
    initConf();
    getPath();
    
    //Set zeit to current time    
    timeSet();
    int statNum = getStatNum();

    int i = 0;
    for (i = 0; i < statNum; i++) {
        //Make Pointer point to current status
        statsPtr = &stats[i]; 
        //Load Settings into Struct
        getConfList(statsPtr, i);
        getConfEnable(statsPtr);
        if (stats[i].enabled) {
            getConfRegex(statsPtr);
            getConfCmmd(statsPtr);
            cmmdOutput(statsPtr);
            regexing(statsPtr);
            makeJansson(statsPtr);
        }
    }

    //Destroy Config
    destroyConf(); 
    return 0;
}

void timeSet() {
    time_t epoch_time;
    struct tm *tm_p;
    epoch_time = time( NULL );
    tm_p = localtime( &epoch_time );
    sprintf(zeit, "%.2d:%.2d", 
    tm_p->tm_hour, tm_p->tm_min );
}

void cmmdOutput(Status *stat) {
    char raw[OUTPUT_SIZE];
    FILE *fp;

    fp = popen(stat->cmmd, "r");  
    while(fgets(raw, sizeof(stat->raw-1), fp) != NULL) {
        strcat(stat->raw,raw);
    }
    pclose(fp);
}


