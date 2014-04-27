#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <libconfig.h>
#include "main.h"


config_t config;
config_setting_t *setting;
char path[OUTPUT_SIZE];


//get the config specified in main.h
void initConf () {
    //init config Structure
    config_init(&config);
    //parse File and watch for Errors
    if(! config_read_file(&config, CFG_FILE))
    {
        fprintf(stderr, "%s:%d - %s\n", config_error_file(&config),
        config_error_line(&config), config_error_text(&config));
        config_destroy(&config);
        exit(1);
    }
}

//Get Path to .jsons
void getPath() {
    const char *temp;
    config_lookup_string(&config, "path", &temp);
    strcpy(path, temp);
}

//get number of Stats
int getStatNum () {
    setting = config_lookup(&config, "list");
    return config_setting_length(setting);
}


//Set name of *stat to the ith item of config list
void getConfList(Status *stat, int i) {
    setting = config_lookup(&config, "list");
    stat->name = config_setting_get_string_elem(setting, i);
}

//Check if *stat is enabled 
void getConfEnable(Status *stat) {
    setting = config_lookup(&config, stat->name); 
    config_setting_lookup_bool(setting, "enabled", &stat->enabled);
}

//Get Command of *stat
void getConfCmmd(Status *stat) {
    setting = config_lookup(&config, stat->name); 
    config_setting_lookup_string(setting, "cmmd", &stat->cmmd);
}
 
//Get Regex of *stat
void getConfRegex(Status *stat) {
    setting = config_lookup(&config, stat->name); 
    config_setting_lookup_string(setting, "regex", &stat->regex);
}

//Get Type of *stat
void getConfType(Status *stat) {
    setting = config_lookup(&config, stat->name); 
    config_setting_lookup_int(setting, "type", &stat->type);
}

//Destroy Config
void destroyConf() {
    config_destroy(&config); 
}
