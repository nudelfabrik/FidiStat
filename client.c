#include "client.h"
#include "config.h"
#include "json.h"
#include "bootstrap.h"
#include "tls.h"
#include "main.h"

void handleSigterm(int sig) {
    term = 1;
}

void client(commandType type) {
    // open Log
    fprintf(stdout, "Starting fidistat...\n");
    openlog("fidistat-client", LOG_PID, LOG_DAEMON);
    syslog(LOG_INFO, "Started Fidistat Client");

    // load Config File and Settings
    initConf();
    if (!getLocal()) {
        initTLS();
    }

    // Setup all config files
    Status stats[getStatNum()];
    Status *statsPtr;
    confSetup(stats);

    // manual resend the displaysettings
    if (type == UPDT) {
        for (int i = 0; i < getStatNum(); i++) {
            createFile(&stats[i], UPDATE);
        }
        return;
    }
    
    // Delete all files from this host
    if (type == DEL) {
        if (!getLocal()) {
            json_t *list = json_array();
            for (int i = 0; i < getStatNum(); i++) {
                json_array_append_new(list, json_string(stats[i].name));
            }
            struct tls* ctx = initCon(DELETE, getStatNum());
            char * payloadStr = json_dumps(list, JSON_COMPACT);
            sendOverTLS(ctx, payloadStr);
            free(payloadStr);
            tls_close(ctx);
            tls_free(ctx);
        } else {
            for (int i = 0; i < getStatNum(); i++) {
                delete(getClientName(), stats[i].name);
            }
        }
        return;
    }

    // Check server if he needs a new .json
    if (!getLocal()) {
        sendHello(stats);
    }

    // Destroy Config
    destroyConf(); 

    //daemonize
    struct pidfh *pfh;
    if (type == START && !now_flag) {
        pfh = daemon_start('c');
    }

    // flags
    if (now_flag) {
        syslog(LOG_INFO, "Running once");
        sleep(1);
    } else {
        fixtime();
    }

// MAIN LOOP
    signal(SIGTERM, handleSigterm);
    while(!term) {
        // Set zeit to current time    
        timeSet();
        // Main Loop, go over every Status
        for (int i = 0; i < getStatNum(); i++) {
            //Make Pointer point to current status
            statsPtr = &stats[i]; 

            if (statsPtr != NULL) {
                if (statsPtr->enabled) {
                    // Execute Command and save Output
                    processCommand(statsPtr);
                }
            }
        }
        // Send Status to Server
        sendStat(stats, getStatNum());

        // Run only once
        if (now_flag) {
            break;
        }

        if (!term) {
            sleep(getInterval() * 60);
        }
    }
// MAIN LOOP
    if (!getLocal()) {
        deinitTLS();
    }
    if (type == START && !now_flag) {
        pidfile_remove(pfh);
    }

    syslog(LOG_INFO, "Stopped Fidistat Client");
    closelog();
    exit(0);
}

// Send all available settings, bootstrap if necessary
void sendHello(Status stat[]) {

    // Compose list
    json_error_t error;
    json_t *list = json_array();
    for (int i = 0; i < getStatNum(); i++) {
        json_array_append_new(list, json_string(stat[i].name));
    }

    // Send header
    struct tls* ctx = initCon(HELLO, getStatNum());
    char * payloadStr = json_dumps(list, JSON_COMPACT);
    sendOverTLS(ctx, payloadStr);
    free(payloadStr);

    // check answer and bootstrap necessary files
    json_t* relist = recvOverTLS(ctx);
    for (int i = 0; i < json_array_size(relist); i++) {
        const char *name = json_string_value(json_array_get(relist, i));
        for (int j = 0; j < getStatNum(); j++) {
            if (strcmp(stat[j].name, name) == 0) {
                createFile(&stat[j], CREATE);
                break;
            }
        }
    }

    tls_close(ctx);
    tls_free(ctx);

}

// dump/send all Stats
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
    if (getLocal()) {
        for (int i = 0; i < statNum; i++) {
            if (stats[i].enabled) {
                pasteJSON(arrays[i], getClientName());
            }
        }
    } else {
        int statActive = 0;
        for (int i = 0; i < statNum; i++) {
            if (stats[i].enabled) {
                statActive++;
            }
        }

        struct tls* ctx = initCon(NEWDATA, statActive);

        for (int i = 0; i < statNum; i++) {
            if (stats[i].enabled) {
                char * payloadStr = json_dumps(arrays[i], JSON_COMPACT);
                sendOverTLS(ctx, payloadStr);
                free(payloadStr);
            }
        }

        tls_close(ctx);
        tls_free(ctx);
    }
}

// Send header over TCP
struct tls* initCon(connType type, int size) {
    // Create Header object
    json_t *header = json_object();
    json_object_set(header, "from", json_string(getClientName()));
    json_object_set(header, "type", json_integer(type));
    json_object_set(header, "size", json_integer(size));
    char * headerStr = json_dumps(header, JSON_COMPACT | JSON_REAL_PRECISION(5));

    // Initiate TLS Session
    struct tls* ctx = tls_client();
    tls_configure(ctx, tlsClient_conf);

    if (tls_connect(ctx, getClientServerURL(), getClientServerPort()) == -1) {
        syslog(LOG_ERR, "%s\n", tls_error(ctx));
        return NULL;
    }
    // Send Header
    sendOverTLS(ctx, headerStr);
    free(headerStr);
    return ctx;

}

// Read config of Stats
void confSetup(Status stats[]) {
    int i = 0;
    for (i = 0; i < getStatNum(); i++) {
        Status newStat;
        setConfName(&newStat, i);
        setConfEnable(&newStat);

        if (newStat.enabled) {
            // Load Remaining Config Settings
            setConfType(&newStat);
            setConfCmmd(&newStat);

            if (newStat.type == 2) {
                setCSVtitle(&newStat);
            } else {
                setConfNum(&newStat);
            }

            // Create File if not present
            if (getLocal()) {
                bootstrap(&newStat);
            }
        }
        stats[i] = newStat;
    }
}

void initTLS(void) {
    tls_init();
    tlsClient_conf = tls_config_new();
    tls_config_set_cert_file(tlsClient_conf, getClientCertFile_v());
    tls_config_set_ca_file(tlsClient_conf, getClientCertFile_v());
    tls_config_insecure_noverifyname(tlsClient_conf);
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
    // If 60 minutes dividable by interval
    if ((60 % getInterval()) == 0) {
        if ( (tm_p->tm_min % getInterval()) != 0) {
            int min = getInterval() - (tm_p->tm_min % getInterval());
            sleep(min*60);
        }
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
    if( stat->type == 2) {
        return 2;
    }
    stat->result = (float *) malloc(stat->num * sizeof(float));

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

    if (verbose_flag) {
        syslog(LOG_INFO, "Successfully processed command %d:", stat->id);
    }

    return 0;
}
