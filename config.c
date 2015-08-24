#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <libconfig.h>
#include "bootstrap.h"
#include "client.h"
#include "config.h"

//get the config specified in main.h
void initConf (const char * path) {
    //init config Structure
    config_init(&config);
    Settings set;
    setting = &set;
    //parse File and watch for Errors
    if(! config_read_file(&config, path))
    {
        syslog(LOG_ERR, "%s:%d - %s\n", config_error_file(&config),
        config_error_line(&config), config_error_text(&config));
        config_destroy(&config);
        exit(1);
    }
    // fill in the SettingsStruct
    const char *local;

    //get Path to json
    if (!config_lookup_string(&config, "path", &local)) {
        syslog(LOG_ERR, "Can't lookup path to .json\n");
        exit(1);
    }
    set.path = strdup(local);
    if (path[strlen(path)-1] != '/') {
        syslog(LOG_ERR, "Path does not end with /\n");
        exit(1);
    }
 
    // Number of Stats
    set.statNum = config_setting_length(getSetting("list"));

    //getMaxCount();
    if (!config_lookup_int(&config, "maxEntrys", &set.maxCount)) {
        syslog(LOG_ERR, "Can't find maxEntries");
        exit(1);
    }

    //getLocalBool();
    if (!config_lookup_bool(&config, "local", &set.local)) {
        syslog(LOG_ERR, "Can't find local");
        exit(1);
    }

    //getClientName();
    if (!config_lookup_string(&config, "clientName", &local)) {
        syslog(LOG_ERR, "Can't lookup name\n");
        exit(1);
    }
    set.clientName = strdup(local);

    //getServerPort();
    if (!config_lookup_string(&config, "ServerPort", &local)) {
        syslog(LOG_ERR, "Can't lookup Server Port\n");
        exit(1);
    }
    set.serverPort = strdup(local);

    //getClientServerURL();
    if (!config_lookup_string(&config, "serverURL", &local)) {
        syslog(LOG_ERR, "Can't lookup name\n");
        exit(1);
    }
    set.serverURL = strdup(local);

}

int getStatNum() {
    return setting->statNum
}
const char* getPath() {
    return setting->path
}
const char* getClientName() {
    return setting->clientName;
}
int getMaxCount() {
    return setting->maxCount;
}
int getLocal() {
    return setting->local;
}
const char* getServerPort() {
    return setting->serverPort;
}
const char* getClientServerURL() {
    return setting->serverURL;
}

const char* getClientCertFile_v() {
    const char *lfile;
    if (!config_lookup_string(&config, "clientCertFile", &lfile)) {
        syslog(LOG_ERR, "Can't lookup Client Cert\n");
        exit(1);
    }
    return lfile;
}

const char* getServerCertFile_v() {
    const char *lfile;
    if (!config_lookup_string(&config, "ServerCertFile", &lfile)) {
        syslog(LOG_ERR, "Can't lookup Server Cert\n");
        exit(1);
    }
    return lfile;
}
int getServerIPv6_v() {
    int v6;
    if (!config_lookup_int(&config, "maxEntrys", v6)) {
        syslog(LOG_ERR, "Can't find maxEntries");
        exit(1);
    }
    return v6;
}


// Functions needed for building Status
//-------------------------------------

config_setting_t* getSetting(const char * item) {
    config_setting_t *setting = config_lookup(&config, item);
    if (setting == NULL) {
        syslog(LOG_ERR, "Can't find %s\n", item);
        exit(1);
    }
    return setting;
}

//Set name of *stat to the ith item of config list
void setConfName(Status *stat, int i) {
    stat->name = strdup(config_setting_get_string_elem(getSetting("list"), i));
}

//Check if *stat is enabled 
void setConfEnable(Status *stat) {
    if (!config_setting_lookup_bool(getSetting(stat->name), "enabled", &stat->enabled)) {
        syslog(LOG_ERR, "Can't lookup enabled of %s\n", stat->name);
        exit(1);
    }
}

//Get Command of *stat
void setConfCmmd(Status *stat) {
    const char *lcmmd;
    if (!config_setting_lookup_string(getSetting(stat->name), "cmmd", &lcmmd)) {
        syslog(LOG_ERR, "Can't lookup Command of %s\n", stat->name);
        exit(1);
    }
    stat->cmmd = strdup(lcmmd);
}
 
void setCSVtitle(Status *stat) {
    const char* title;
    config_setting_t *setting = config_lookup(&config, stat->name);
    if (!config_setting_lookup_string(config_setting_get_member(setting, "display"), "title", &title)) {
        syslog(LOG_ERR, "Can't lookup Title of %s\n", stat->name);
        exit(1);
    }
    stat->csv = title;
}

//Get Type of *stat
void setConfType(Status *stat) {
    const char* type;
    config_setting_t *setting = config_lookup(&config, stat->name);
    if (!config_setting_lookup_string(config_setting_get_member(setting, "display"), "type", &type)) {
        syslog(LOG_ERR, "Can't lookup Config Type of %s\n", stat->name);
        exit(1);
    }
    if (!strcmp(type,  "line")) {
        stat->type = 0;
    } else {
        if (!strcmp(type,  "bar")) {
            stat->type = 1;
        } else {
            if (!strcmp(type,  "csv")) {
                stat->type = 2;
            } else {
                syslog(LOG_ERR, "Config Type of  %s\n not recognized", stat->name);
                exit(1);
            }
        }
    }
}

//Destroy Config
void destroyConf() {
    config_destroy(&config);
}
