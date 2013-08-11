#ifndef CONFIG
#define CONFIG
void initConf();
void destroyConf();
const char * getConfList(Status *Stat, int i);
int getStatNum();
void getConfEnable(Status *Stat);
void getConfCmmd(Status *Stat);
void getConfRegex(Status *Stat);

#endif
