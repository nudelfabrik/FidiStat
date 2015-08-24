#ifndef CONFIG
#define CONFIG
#include "main.h"
#include <libconfig.h>

struct settingsStruct {
    const char *path;
    int statNum;
    int maxCount;
    int local;
    const char *clientName;
    const char *serverURL;
    const char *serverPort;
};

typedef struct settingsStruct Settings;

static config_t config;
static Settings* setting;

void initConf(const char * path);
void destroyConf();

config_setting_t* getSetting(const char* item);

int getStatNum();
const char* getPath();
const char* getClientName();
int getMaxCount();
int getLocal();
const char* getClientServerPort();
const char* getClientServerURL();
const char* getClientCertFile_v();

int getServerIPv6_v();
const char* getServerCertFile_v();

void setConfName(Status *Stat, int i);
void setConfEnable(Status *Stat);
void setConfType(Status *Stat);
void setConfCmmd(Status *Stat);
void setConfRegex(Status *Stat);
void setCSVtitle(Status *stat);
#endif
