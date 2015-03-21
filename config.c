#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <libconfig.h>
#include "main.h"
#include "bootstrap.h"

config_t config;

//get the config specified in main.h
void initConf (const char * path) {
    //init config Structure
    config_init(&config);
    //parse File and watch for Errors
    if(! config_read_file(&config, path))
    {
        fprintf(stderr, "%s:%d - %s\n", config_error_file(&config),
        config_error_line(&config), config_error_text(&config));
        config_destroy(&config);
        exit(1);
    }
}

config_setting_t* getSetting(const char * item) {
    config_setting_t *setting = config_lookup(&config, item);
    if (setting == NULL) {
        fprintf(stderr, "Can't find %s\n", item);
        exit(1);
    }
    return setting;
}
//Get Path to .jsons
void getPath() {
    if (!config_lookup_string(&config, "path", &path)) {
        fprintf(stderr, "Can't lookup path to .json\n");
        exit(1);
    }
    if (path[strlen(path)-1] != '/') {
        fprintf(stderr, "Path does not end with /\n");
        exit(1);
    }
}

// Get max datapoints displayed
void getMaxCount() {
    if (!config_lookup_int(&config, "maxEntrys", &maxCount)) {
        fprintf(stderr, "Can't find maxEntries");
        exit(1);
    }
}

//get number of Stats
int getStatNum () {
    getSetting("list"); 
    return config_setting_length(getSetting("list"));
}


//Set name of *stat to the ith item of config list
void getConfList(Status *stat, int i) {
    stat->name = config_setting_get_string_elem(getSetting("list"), i);
}

//Check if *stat is enabled 
void getConfEnable(Status *stat) {
    if (!config_setting_lookup_bool(getSetting(stat->name), "enabled", &stat->enabled)) {
        fprintf(stderr, "Can't lookup enabled of %s\n", stat->name);
        exit(1);
    }
}

//Get Command of *stat
void getConfCmmd(Status *stat) {
    if (!config_setting_lookup_string(getSetting(stat->name), "cmmd", &stat->cmmd)) {
        fprintf(stderr, "Can't lookup Command of %s\n", stat->name);
        exit(1);
    }
}
 
//Get Regex of *stat
void getConfRegex(Status *stat) {
    if (!config_setting_lookup_string(getSetting(stat->name), "regex", &stat->regex)) {
        fprintf(stderr, "Can't lookup Regex of %s\n", stat->name);
        exit(1);
    }
}

//Get Type of *stat
void getConfType(Status *stat) {
    if (!config_setting_lookup_int(getSetting(stat->name), "type", &stat->type)) {
        fprintf(stderr, "Can't lookup Config Type of %s\n", stat->name);
        exit(1);
    }
}

void getDisplaySettings(const char* path, const char* subSetting) {

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
            const char* name = config_setting_name(sett);
            addNewSubSetting(name);

            // Add all other 
            char newPath[strlen(path)+strlen(name)+2];
            sprintf(newPath, "%s.%s" , path, name);
            getDisplaySettings(newPath, name);
        }
    }
}

//Destroy Config
void destroyConf() {
    config_destroy(&config); 
}
