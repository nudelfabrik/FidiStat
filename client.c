#include <syslog.h>
#include <string.h>
#include <regex.h>
#include <stdio.h>
#include <time.h>
#include "client.h"
#include "config.h"
#include "json.h"
#include "bootstrap.h"
// Default Config Location

char *cfgLocation = "/usr/local/etc/fidistat/config.cfg";
void client(void) {

    // load Config File and Settings
    initConf(cfgLocation);

    // Get max number of Settings
    int statNum = getStatNum();

    Status stats[statNum];
    Status *statsPtr;

    // Setup all config files
    confSetup(stats);

    if (!(dry_flag)) {
        fixtime();
    }
    while(1) {
        // Set zeit to current time    
        timeSet();
        // Main Loop, go over every Status
        for (int i = 0; i < statNum; i++) {
            //Make Pointer point to current status
            statsPtr = &stats[i]; 

            if (statsPtr != NULL) {
                syslog(LOG_DEBUG, "checking: %s", statsPtr->name);
                if (statsPtr->enabled) {
                    // Execute Command and save Output
                    cmmdOutput(statsPtr);
                    // Send Status to Server
                    sendStat(statsPtr);
                }
            }
        }
        sleep(600);
    }
}

void sendStat(Status *stat) {
    if (local) {
        makeStat(stat);
    } else {
        //open Socket and Send
    }

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
            syslog(LOG_INFO, "Removed %s.json\n", newStat.name);
        } else {
            if (newStat.enabled) {
                if (clean_flag) {
                    del(&newStat);
                    fprintf(stdout, "Removed %s.json\n", newStat.name);
                }

                syslog(LOG_DEBUG, "added: %s", newStat.name);
                // Load Remaining Config Settings
                setConfType(&newStat);
                setConfCmmd(&newStat);
                setConfRegex(&newStat);

                // Create File if not present
                bootstrap(&newStat);
            }
            stats[i] = newStat;
        }
            
    }

    for (i = 0; i < getStatNum(); i++) {
        syslog(LOG_DEBUG, "%i: %s", i,stats[i].name);
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
int processCommand(Status *stat) {
    char raw[OUTPUT_SIZE] = "";
    char *rawPnt = raw;
    FILE *fp;

    fp = popen(stat->cmmd, "r");  
    if (fgets(raw, sizeof(raw-1), fp) == NULL) {
        syslog(LOG_ERR, "Error executing command %s\n", stat->name);
        return -1;
    }
    if (pclose(fp) != 0) {
        syslog(LOG_ERR, "Command of %s exits != 0\n", stat->name);
        return -1;
    }
    regex_t regex;
    int retex;
    regmatch_t pmatch;
    char res[OUTPUT_SIZE];

    int j = 0;

    retex = regcomp( &regex, stat->regex, REG_EXTENDED);
    if (retex) { 
        syslog(LOG_ERR, "Could not compile regex\n"); 
        return -1; 
    }
    while (regexec(&regex, rawPnt, 1, &pmatch, 0) == 0) {
        sprintf(res, "%.*s",  (int)(pmatch.rm_eo - pmatch.rm_so), raw[pmatch.rm_so]);
        double value = atof(res);
        stat->result[j] = &value;
        rawPnt += pmatch.rm_eo;
        j++;
    }
    regfree(&regex);
    return 0;
}

void setLocation(char* loc) {
    cfgLocation = loc;
}


void del(Status *stat) {
    char file[strlen(path) + strlen(stat->name) + 6];
    sprintf(file, "%s%s.json", path, stat->name);
    remove(file);
}
