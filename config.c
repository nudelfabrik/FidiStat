#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <libconfig.h>
#include "main.h"


config_t config;
config_setting_t *setting;
char path[OUTPUT_SIZE];


void initConf () {
    config_init(&config);
    if(! config_read_file(&config, CFG_FILE))
    {
        fprintf(stderr, "%s:%d - %s\n", config_error_file(&config),
        config_error_line(&config), config_error_text(&config));
        config_destroy(&config);
        exit(1);
    }
}

void getPath() {
    const char *temp;
    config_lookup_string(&config, "path", &temp);
    strcpy(path, temp);
}

int getStatNum () {
    setting = config_lookup(&config, "list");
    return config_setting_length(setting);
}


void getConfList(Status *stat, int i) {
    setting = config_lookup(&config, "list");
    stat->name = config_setting_get_string_elem(setting, i);
}

void getConfEnable(Status *stat) {
    setting = config_lookup(&config, stat->name); 
    config_setting_lookup_bool(setting, "enabled", &stat->enabled);
}

void getConfCmmd(Status *stat) {
    setting = config_lookup(&config, stat->name); 
    config_setting_lookup_string(setting, "cmmd", &stat->cmmd);
}

void getConfRegex(Status *stat) {
    setting = config_lookup(&config, stat->name); 
    config_setting_lookup_string(setting, "regex", &stat->regex);
}

void destroyConf() {
    config_destroy(&config); 
}
