#ifndef CONFIG
#define CONFIG
void initConf(const char * path);
void destroyConf();
void getConfList(Status *Stat, int i);
int getStatNum ();
const char* getStatName(int i);
void getPath();
void getMaxCount();
void getConfEnable(Status *Stat);
void getConfType(Status *Stat);
void getConfCmmd(Status *Stat);
void getConfRegex(Status *Stat);
void getDisplaySettings(const char* name, const char* path);
#endif
