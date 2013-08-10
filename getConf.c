#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <libconfig.h>
#include "main.h"


config_t config;
config_setting_t *setting;
struct statStruct stats[10];

void initConf () {
    config_init(&config);
    if(! config_read_file(&config, CFG_FILE))
    {
        fprintf(stderr, "%s:%d - %s\n", config_error_file(&config),
        config_error_line(&config), config_error_text(&config));
        config_destroy(&config);
    }
}
int getStatNum () {
    setting = config_lookup(&config, "list");
    return config_setting_length(setting);
}


void getConfList(int i) {
    setting = config_lookup(&config, "list");
    stats[i].name = config_setting_get_string_elem(setting, i);
}

void getConfEnable(int i) {
    setting = config_lookup(&config, stats[i].name); 
    config_setting_lookup_bool(setting, "enabled", &stats[i].enabled);
}

void getConfCmmd(int i) {
    setting = config_lookup(&config, stats[i].name); 
    config_setting_lookup_string(setting, "cmmd", &stats[i].cmmd);
}

void getConfRegex(int i) {
    setting = config_lookup(&config, stats[i].name); 
    config_setting_lookup_string(setting, "regex", &stats[i].regex);
}

void destroyConf() {
    config_destroy(&config); 
}
