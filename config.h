#ifndef CONFIG
#define CONFIG
#include "main.h"

void initConf(const char * path);
void destroyConf();
int getStatNum ();
void setConfName(Status *Stat, int i);
const char* getStatName(int i);
void getPath();
void getMaxCount();
void getLocalBool();
void setConfEnable(Status *Stat);
void setConfType(Status *Stat);
void setConfCmmd(Status *Stat);
void setConfRegex(Status *Stat);
const char* getCSVtitle(Status *stat);
void getDisplaySettings(const char* name, const char* subSetting);
void getSequences(const char* name);
void getBarTitles(const char* name);
#endif
