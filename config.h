#ifndef CONFIG
#define CONFIG
#include "main.h"
#include <libconfig.h>

extern config_t config;
void initConf(const char * path);
void destroyConf();
int getStatNum();
void getPath();
void getMaxCount();
void getLocalBool();
int getIPv6Bool();
void getClientName();
const char* getClientCertFile();
const char* getServerCertFile();
void getServerPort();
void getClientServerURL();
void setConfName(Status *Stat, int i);
void setConfEnable(Status *Stat);
void setConfType(Status *Stat);
void setConfCmmd(Status *Stat);
void setConfRegex(Status *Stat);
void setCSVtitle(Status *stat);
#endif
