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
void setConfEnable(Status *Stat);
void setConfType(Status *Stat);
void setConfCmmd(Status *Stat);
void setConfRegex(Status *Stat);
const char* getCSVtitle(Status *stat);
void getDisplaySettings(const char* name, const char* path);
void getSequences(const char* path);
void getBarTitles(const char* path);
#endif
