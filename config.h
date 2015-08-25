#ifndef CONFIG
#define CONFIG
#include "main.h"
#include <libconfig.h>

struct settingsStruct {
    const char *path;
    int statNum;
    int maxCount;
    int local;
    int interval;
    const char *clientName;
    const char *serverURL;
    const char *serverPort;
};

typedef struct settingsStruct Settings;

static config_t config;
static Settings setting;

void initConf(const char * path);
void destroyConf();


int getStatNum();
const char* getPath();
const char* getClientName();
int getMaxCount();
int getLocal();
int getInterval();
const char* getClientServerPort();
const char* getClientServerURL();
const char* getClientCertFile_v();

int getServerIPv6_v();
const char* getServerCertFile_v();

config_setting_t* getSetting(int id);
void setConfName(Status *Stat, int i);
void setConfEnable(Status *Stat);
void setConfType(Status *Stat);
void setConfCmmd(Status *Stat);
void setConfNum(Status *Stat);
void setConfRegex(Status *Stat);
void setCSVtitle(Status *stat);
config_setting_t* getLookup(const char *path);
#endif
