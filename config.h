#ifndef CONFIG
#define CONFIG
void initConf(const char * path);
void destroyConf();
const char * getConfList(Status *Stat, int i);
int getStatNum();
void getPath();
void getMaxCount();
void getConfEnable(Status *Stat);
void getConfType(Status *Stat);
void getConfCmmd(Status *Stat);
void getConfRegex(Status *Stat);

#endif
