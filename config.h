#ifndef CONFIG
#define CONFIG
#include <libconfig.h>
void initConf();
void destroyConf();
const char * getConfList(Status *Stat, int i);
config_setting_t* getSetting(const char * item);
int getStatNum();
void getPath();
void getConfEnable(Status *Stat);
void getConfType(Status *Stat);
void getConfCmmd(Status *Stat);
void getConfRegex(Status *Stat);

#endif
