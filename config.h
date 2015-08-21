#ifndef CONFIG
#define CONFIG
#include "main.h"

void initConf(const char * path);
void destroyConf();
int getStatNum ();
void getPath();
void getMaxCount();
void getLocalBool();
void getClientName();
const char* getClientCertFile();
const char* getClientServerURL();
void setConfName(Status *Stat, int i);
void setConfEnable(Status *Stat);
void setConfType(Status *Stat);
void setConfCmmd(Status *Stat);
void setConfRegex(Status *Stat);
void setCSVtitle(Status *stat);
void getDisplaySettings(const char* name, const char* subSetting);
void getSequences(const char* name);
void getBarTitles(const char* name);
#endif
