#include <libconfig.h>
#include "bootstrap.h"
#include "client.h"
#include "config.h"

static char *cfgLocation = "/usr/local/etc/fidistat/config.cfg";
//get the config specified in main.h
void initConf () {
    //init config Structure
    config_init(&config);
    //parse File and watch for Errors
    if(! config_read_file(&config, cfgLocation))
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
    setting.path = strdup(local);

    // Path must have trailing /"
    if (local[strlen(local)-1] != '/') {
        char fixed[strlen(local+1)];
        sprintf(fixed, "%s/\n", local);
        setting.path = strdup(fixed);
    }
 
    // Number of Stats
    int nums = config_setting_length(config_lookup(&config, "settings"));
    setting.statNum = nums;

    //getMaxCount();
    if (!config_lookup_int(&config, "maxEntrys", &setting.maxCount)) {
        syslog(LOG_ERR, "Can't find maxEntries");
        exit(1);
    }

    // Interval of CLient
    if (!config_lookup_int(&config, "interval", &setting.interval)) {
        syslog(LOG_ERR, "Can't find interval");
        exit(1);
    }

    //getLocalBool();
    if (!config_lookup_bool(&config, "local", &setting.local)) {
        syslog(LOG_ERR, "Can't find local");
        exit(1);
    }

    //getClientName();
    if (!config_lookup_string(&config, "clientName", &local)) {
        syslog(LOG_ERR, "Can't lookup name\n");
        exit(1);
    }
    setting.clientName = strdup(local);

    //getServerPort();
    if (!config_lookup_string(&config, "ServerPort", &local)) {
        syslog(LOG_ERR, "Can't lookup Server Port\n");
        exit(1);
    }
    setting.serverPort = strdup(local);

    //getClientServerURL();
    if (!config_lookup_string(&config, "serverURL", &local)) {
        syslog(LOG_ERR, "Can't lookup name\n");
        exit(1);
    }
    setting.serverURL = strdup(local);

}

int getStatNum() {
    return setting.statNum;
}
const char* getPath() {
    return setting.path;
}
const char* getClientName() {
    return setting.clientName;
}
int getMaxCount() {
    return setting.maxCount;
}
int getLocal() {
    return setting.local;
}
int getInterval() {
    return setting.interval;
}
const char* getClientServerPort() {
    return setting.serverPort;
}
const char* getClientServerURL() {
    return setting.serverURL;
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
    if (!config_lookup_bool(&config, "ipv6", &v6)) {
        syslog(LOG_ERR, "Can't find ipv6");
        exit(1);
    }
    return v6;
}


// Functions needed for building Status
//-------------------------------------

config_setting_t* getSetting(int id) {
    config_setting_t *setting = config_setting_get_elem(config_lookup(&config, "settings"), id);
    if (setting == NULL) {
        syslog(LOG_ERR, "Can't find %d\n", id);
        exit(1);
    }
    return setting;
}

//Set name of *stat to the ith item of config list
void setConfName(Status *stat, int i) {
    stat->id = i;
    const char *lname;
    if (!config_setting_lookup_string(getSetting(stat->id), "name", &lname)) {
        syslog(LOG_ERR, "Can't lookup Name of %s\n", stat->name);
        exit(1);
    }
    stat->name = strdup(lname);
}

//Check if *stat is enabled 
void setConfEnable(Status *stat) {
    if (!config_setting_lookup_bool(getSetting(stat->id), "enabled", &stat->enabled)) {
        syslog(LOG_ERR, "Can't lookup enabled of %s\n", stat->name);
        exit(1);
    }
}

//Get Command of *stat
void setConfCmmd(Status *stat) {
    const char *lcmmd;
    if (!config_setting_lookup_string(getSetting(stat->id), "cmmd", &lcmmd)) {
        syslog(LOG_ERR, "Can't lookup Command of %s\n", stat->name);
        exit(1);
    }
    stat->cmmd = strdup(lcmmd);
}

void setConfNum(Status *stat) {
    char path[25];
    sprintf(path, "settings.[%d].sequencetitles" , stat->id);
    stat->num = config_setting_length(getLookup(path));
}
 
void setCSVtitle(Status *stat) {
    const char* title;
    config_setting_t *setting = getSetting(stat->id);
    if (!config_setting_lookup_string(config_setting_get_member(setting, "display"), "title", &title)) {
        syslog(LOG_ERR, "Can't lookup Title of %s\n", stat->name);
        exit(1);
    }
    stat->csv = title;
}

//Get Type of *stat
void setConfType(Status *stat) {
    const char* type;
    config_setting_t *setting = getSetting(stat->id);
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

config_setting_t* getLookup(const char *path) {
    return config_lookup(&config, path);
}

void setLocation(char *loc) {
    cfgLocation = loc;
}

//Destroy Config
void destroyConf() {
    config_destroy(&config);
}
