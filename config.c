#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <libconfig.h>
#include "bootstrap.h"
#include "client.h"
#include "config.h"

config_t config;

//get the config specified in main.h
void initConf (const char * path) {
    //init config Structure
    config_init(&config);
    //parse File and watch for Errors
    if(! config_read_file(&config, path))
    {
        syslog(LOG_ERR, "%s:%d - %s\n", config_error_file(&config),
        config_error_line(&config), config_error_text(&config));
        config_destroy(&config);
        exit(1);
    }
    getPath();
    getMaxCount();

}

config_setting_t* getSetting(const char * item) {
    config_setting_t *setting = config_lookup(&config, item);
    if (setting == NULL) {
        syslog(LOG_ERR, "Can't find %s\n", item);
        exit(1);
    }
    return setting;
}

//Get Path to .jsons
void getPath() {
    if (!config_lookup_string(&config, "path", &path)) {
        syslog(LOG_ERR, "Can't lookup path to .json\n");
        exit(1);
    }
    if (path[strlen(path)-1] != '/') {
        syslog(LOG_ERR, "Path does not end with /\n");
        exit(1);
    }
}

// Get max datapoints displayed
void getMaxCount() {
    if (!config_lookup_int(&config, "maxEntrys", &maxCount)) {
        syslog(LOG_ERR, "Can't find maxEntries");
        exit(1);
    }
}

//get number of Stats
int getStatNum () {
    return config_setting_length(getSetting("list"));
}


//Set name of *stat to the ith item of config list
void setConfName(Status *stat, int i) {
    stat->name = config_setting_get_string_elem(getSetting("list"), i);
    stat->ident = config_setting_get_string(getSetting("identifier"));
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
    if (!config_setting_lookup_string(getSetting(stat->name), "cmmd", &stat->cmmd)) {
        syslog(LOG_ERR, "Can't lookup Command of %s\n", stat->name);
        exit(1);
    }
}
 
//Get Regex of *stat
void setConfRegex(Status *stat) {
    if (stat->type != 2) {
        if (!config_setting_lookup_string(getSetting(stat->name), "regex", &stat->regex)) {
            syslog(LOG_ERR, "Can't lookup Regex of %s\n", stat->name);
            exit(1);
        }
    }
}

const char* getCSVtitle(Status *stat) {
    const char* title;
    config_setting_t *setting = config_lookup(&config, stat->name);
    if (!config_setting_lookup_string(config_setting_get_member(setting, "display"), "title", &title)) {
        syslog(LOG_ERR, "Can't lookup Title of %s\n", stat->name);
        exit(1);
    }
    return title;
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

// Load Display Setting from path
void getDisplaySettings(const char* name, const char* subSetting) {

    char path[strlen(name)+20];
    if (!strcmp(subSetting,  "graph")) {
        sprintf(path, "%s.display" , name);
    } else {
        sprintf(path, "%s.display.%s", name, subSetting);
    }
    // Get Display Setting of name
    config_setting_t* display = config_lookup(&config, path);
    int numSettings = config_setting_length(display);

    // Add every Setting of Display to json file
    for (int i = 0; i < numSettings; i++) {
        config_setting_t* sett = config_setting_get_elem(display, i);

        // Int and String Settings
        if (config_setting_type(sett) == CONFIG_TYPE_INT) {
            addNewInt(config_setting_name(sett),config_setting_get_int(sett), subSetting);
        }
        if (config_setting_type(sett) == CONFIG_TYPE_STRING) {
            addNewString(config_setting_name(sett),config_setting_get_string(sett), subSetting);
        }

        // Object Settings
        if (config_setting_type(sett) == CONFIG_TYPE_GROUP) {
            const char* subName = config_setting_name(sett);
            addNewSubSetting(subName);

            // Add all other 
            getDisplaySettings(name, subName);
        }
    }
}

// get datasequence arrays
void getSequences(const char* name) {
    char path[strlen(name)+20];
    sprintf(path, "%s.sequencetitles" , name);
    config_setting_t* sequences = config_lookup(&config, path);

    sprintf(path, "%s.sequencecolors" , name);
    config_setting_t* colours = config_lookup(&config, path);

    int numSeq = config_setting_length(sequences);

    // Add every Setting of Display to json file
    for (int i = 0; i < numSeq; i++) {
        if (colours) {
            addNewSequence(config_setting_get_string_elem(sequences, i), config_setting_get_string_elem(colours, i));
        } else {
            addNewSequence(config_setting_get_string_elem(sequences, i), NULL);
        }
    }
}

// get Titles of Bars for Bar Graphs
void getBarTitles(const char* name) {
    char path[strlen(name)+20];
    sprintf(path, "%s.bartitles" , name);
    config_setting_t* sequences = config_lookup(&config, path);
    int numSeq = config_setting_length(sequences);

    for (int i = 0; i < numSeq; i++) {
        addNewBarTitle(config_setting_get_string_elem(sequences, i));
    }

}

//Destroy Config
void destroyConf() {
    config_destroy(&config); 
}
