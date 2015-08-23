#include <syslog.h>
#include <string.h>
#include <regex.h>
#include <stdio.h>
#include <time.h>
#include <pcre.h> 
#include <signal.h>
#include <jansson.h>
#include "client.h"
#include "config.h"
#include "json.h"
#include "bootstrap.h"
#include "tls.h"
// Default Config Location

char *cfgLocation = "/usr/local/etc/fidistat/config.cfg";
volatile sig_atomic_t term = 0;

void handleSigterm(int sig) {
    term = 1;
}
void client(void) {

    // load Config File and Settings
    initConf(cfgLocation);
    initTLS();

    // Get max number of Settings
    int statNum = getStatNum();

    Status stats[statNum];
    Status *statsPtr;

    // Setup all config files
    confSetup(stats);

    // Destroy Config
    destroyConf(); 

    signal(SIGTERM, handleSigterm);

    if (!(dry_flag) && !(now_flag)) {
        fixtime();
    }
    while(!term) {
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
                    processCommand(statsPtr);
                    // Send Status to Server
                }
            }
        }
        sendStat(stats, statNum);
        if (now_flag) {
            break;
        }

        if (!term) {
            sleep(600);
        }
    }
    deinitTLS();
}

void sendStat(Status *stats, int statNum) {
    json_t *arrays[statNum];
    for (int i = 0; i < statNum; i++) {
        //Make Pointer point to current status
        Status *statsPtr = &stats[i]; 

        if (statsPtr != NULL) {
            if (statsPtr->enabled) {
                arrays[i] = makeStat(statsPtr);
            }
        }
    }
    if (local) {
        for (int i = 0; i < statNum; i++) {
            pasteJSON(arrays[i], clientName);
        }
    } else {
        // Create Header object
        json_t *header = json_object();
        json_object_set(header, "from", json_string(clientName));
        json_object_set(header, "type", json_integer(UPDATE));
        json_object_set(header, "size", json_integer(statNum));
        char * headerStr = json_dumps(header, JSON_COMPACT | JSON_REAL_PRECISION(5));

        // Initiate TLS Session
        struct tls* ctx = tls_client();
        tls_configure(ctx, tlsClient_conf);

        if (tls_connect(ctx, "192.168.42.3", "4242") == -1) {
            syslog(LOG_ERR, "%s\n", tls_error(ctx));
            return;
        }
        // Send Header
        sendOverTLS(ctx, headerStr);
        free(headerStr);

        for (int i = 0; i < statNum; i++) {
            char * payloadStr = json_dumps(arrays[i], JSON_COMPACT);
            sendOverTLS(ctx, payloadStr);
            free(payloadStr);
        }

        tls_close(ctx);
        tls_free(ctx);


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
                if (newStat.type == 2) {
                    setCSVtitle(&newStat);
                }


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

void initTLS(void) {
    tls_init();
    tlsClient_conf = tls_config_new();
    syslog(LOG_DEBUG, "Certfile: %s\n", getClientCertFile());
    tls_config_set_cert_file(tlsClient_conf, getClientCertFile());
    tls_config_set_ca_file(tlsClient_conf, getClientCertFile());
    tls_config_insecure_noverifyname(tlsClient_conf);
    serverURL = getClientServerURL();
}
void deinitTLS(void) {
    tls_config_free(tlsClient_conf);
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
    syslog(LOG_INFO, "Processing command");
    if( stat->type == 2) {
        return 2;
    }

    char raw[OUTPUT_SIZE] = "";
    FILE *fp;

    fp = popen(stat->cmmd, "r");  
    int i = 0;
    while (fgets(raw, sizeof(raw)-1, fp) != NULL) {
        stat->result[i] = strtof(raw, NULL);
        i++;
    }
    if (pclose(fp) != 0) {
        syslog(LOG_ERR, "Command of %s exits != 0\n", stat->name);
        return -1;
    }
    syslog(LOG_INFO, "Successfully Loaded command:");

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
