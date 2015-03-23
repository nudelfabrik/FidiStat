/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Benedikt Scholtes
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
#include <getopt.h>
#include "main.h"
#include "regex.h"
#include "config.h"
#include "jansson.h"
#include "bootstrap.h"

const char *helptext="--verbose, -v:                  echoes every value to stdout \n\
--dry, -d:                      Dry run, nothing is written to disk\n\
--config CFG_File, -f CFG_File: expects path to Config file\n\
--clean -c:                     remove all files not enabled\n\
--delete -x:                    remove all files\n"; 

// Default Config Location
char *cfgLocation = "/usr/local/etc/fidistat/config.cfg";
int main(int argc, const char *argv[])
{
    static struct option long_options[] =
    {
        {"verbose", no_argument, &verbose_flag, 1},
        {"dry", no_argument, &dry_flag, 1},
        {"help", no_argument, 0, 'h'},
        {"clean", no_argument, &clean_flag, 1},
        {"delete", no_argument, &delete_flag, 1},
        {"config", required_argument, 0, 'f'},
        {0, 0, 0, 0}
    };
    int option_index = 0;

    int c;

    while ((c = getopt_long(argc, argv, "cdfhvx", long_options, NULL)) != -1) {
        switch (c) {
        case 'v':
            verbose_flag = 1;
            break;
        case 'h':
            fprintf(stdout, "%s", helptext);
            exit(0);
            break;
        case 'd':
            dry_flag = 1;
            break;
        case 'f':
            cfgLocation = optarg;
            break;
        case 'c':
            clean_flag = 1;
            break;
        case 'x':
            delete_flag = 1;
            break;
        case 0:
            break;
        case '?':
        default:     
        fprintf(stderr, "%s: option `-%c' is invalid: ignored\n",
            argv[0], optopt);
            break;
        }

    }


    // load Config File and Settings
    initConf(cfgLocation);
    getPath();
    getMaxCount();

    // Set zeit to current time    
    timeSet();

    // Get max number of Settings
    int statNum = getStatNum();

    Status stats[statNum];
    Status *statsPtr;

    int i = 0;

    if (delete_flag) {
        for (i = 0; i < statNum; i++) {
            getConfList(&stats[i], i);
            del(&stats[i]);
            fprintf(stdout, "Removed %s.json\n", stats[i].name);
        }
        exit(0);
    }
    if (clean_flag) {
        for (i = 0; i < statNum; i++) {
            getConfList(&stats[i], i);
            getConfEnable(&stats[i]);
            del(&stats[i]);
            fprintf(stdout, "Removed %s.json\n", stats[i].name);
        }
        exit(0);
    }

    for (i = 0; i < statNum; i++) {
        //Make Pointer point to current status
        statsPtr = &stats[i]; 
        //Get Name of Status
        getConfList(statsPtr, i);
        getConfEnable(statsPtr);

        if (stats[i].enabled) {
            getConfType(statsPtr);
            bootstrap(statsPtr);
            getConfCmmd(statsPtr);

            // Execute Command and save Output
            cmmdOutput(statsPtr);

            // Check wich type the status is
            if (stats[i].type == 2) {
                if (verbose_flag) {
                    debug(statsPtr);
                }
                makeCSV(statsPtr);
            } else {
                getConfRegex(statsPtr);
                regexing(statsPtr);
                if (verbose_flag) {
                    debug(statsPtr);
                }
                // Add Data to JSON
                makeJansson(statsPtr);
            }
        }
    }

    //Destroy Config
    destroyConf(); 
    return 0;
}

//Set time
void timeSet() {
    time_t epoch_time;
    struct tm *tm_p;
    epoch_time = time( NULL );
    tm_p = localtime( &epoch_time );
    sprintf(zeit, "%.2d:%.2d", 
    tm_p->tm_hour, tm_p->tm_min );
}

//Get Output from Command
void cmmdOutput(Status *stat) {
    char raw[OUTPUT_SIZE] = "";
    FILE *fp;

    strcpy(stat->raw, "\0");

    fp = popen(stat->cmmd, "r");  
    while(fgets(raw, sizeof(stat->raw-1), fp) != NULL) {
        strcat(stat->raw,raw);
    }
    if (pclose(fp) != 0) {
        fprintf(stderr, "Command of %s exits != 0\n", stat->name);
        exit(1);
    }
}

void debug(Status *stat) {
    printf("\nOutput of %s:\n%s\n", stat->name, stat->raw);
    if (stat->type != 2) {
        int i;
        for (i = 0; i < 4; i++) {
            printf("Result %i of %s: %f\n", i, stat->name, stat->result[i]);
        }
    }
}

void del(Status *stat) {
    char file[strlen(path) + strlen(stat->name) + 6];
    sprintf(file, "%s%s.json", path, stat->name);
    remove(file);
}
