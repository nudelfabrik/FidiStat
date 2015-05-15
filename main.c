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
#include <errno.h>
#include <libutil.h>
#include <signal.h>
#include "main.h"
#include "regex.h"
#include "config.h"
#include "json.h"
#include "bootstrap.h"

// Default Config Location
char *cfgLocation = "/usr/local/etc/fidistat/config.cfg";
int main(int argc, const char *argv[])
{
    // Set Flags if some are set
    handleFlags(argc, argv);

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

    // Setup all config files
    confSetup(stats);

    struct pidfh *pfh;
    pid_t otherpid;

    pfh = pidfile_open("/var/run/fidistat.pid", 0600, &otherpid);
    if (pfh == NULL) {
        if (errno == EEXIST) {
            fprintf(stdout, "Daemon already running with PID %d\n", otherpid);
            exit(-1);
        }
            fprintf(stdout, "Cannot create pidfile\n");
    }   

    if  (daemon(0, 0) == -1) {
        fprintf(stderr, "Cannot daemonize");
        pidfile_remove(pfh);
        exit(-1);
    }

    pidfile_write(pfh);

    fixtime();
    int i;
    while(1) {
        timeSet();
        // Main Loop, go over every Status
        for (i = 0; i < statNum; i++) {
            //Make Pointer point to current status
            statsPtr = &stats[i]; 

            if (statsPtr != NULL) {
                if (stats[i].enabled) {
                    // Execute Command and save Output
                    cmmdOutput(statsPtr);
                    makeStat(statsPtr);
                }
            }
        }
        sleep(600);
    }
    pidfile_remove(pfh);

    //Destroy Config
    destroyConf(); 
    return 0;
}

void confSetup(Status stats[]) {
    int i = 0;
    for (i = 0; i < getStatNum(); i++) {
        Status newStat;
        setConfName(&newStat, i);
        setConfEnable(&newStat);

        // delete .jsons if flags are set
        if (delete_flag) {
            del(&newStat);
            fprintf(stdout, "Removed %s.json\n", newStat.name);
        } else {
            if (stats[i].enabled) {
                if (clean_flag) {
                    del(&newStat);
                    fprintf(stdout, "Removed %s.json\n", newStat.name);
                }

                stats[i] = newStat;
                // Load Remaining Config Settings
                setConfType(&stats[i]);
                setConfCmmd(&stats[i]);
                setConfRegex(&newStat);

                // Create File if not present
                bootstrap(&stats[i]);
            }
        }
            
    }
    if (delete_flag || clean_flag) {
        exit(0);
    }

}

// Wait to a round time for execution
void fixtime(void) {
    time_t epoch_time;
    struct tm *tm_p;
    epoch_time = time( NULL );
    tm_p = localtime( &epoch_time );
    // 20 Seconds should be enough to execute all commands
    if (tm_p->tm_sec > 40) {
        sleep(22);
    }
    // TODO: 10 is depened on the interval
    if ( (tm_p->tm_min % 10) != 0) {
        int min = 10 - (tm_p->tm_min % 10);
        sleep(min*60);
    }
    return;
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

void handleFlags(int argc, const char *argv[]) {
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

    int c;
    const char *helptext="--verbose, -v:                  echoes every value to stdout \n\
    --dry, -d:                      Dry run, nothing is written to disk\n\
    --config CFG_File, -f CFG_File: expects path to Config file\n\
    --clean -c:                     remove all files not enabled\n\
    --delete -x:                    remove all files\n"; 

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
}
