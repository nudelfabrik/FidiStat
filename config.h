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
const char* getCSVtitle(Status *stat);
void getDisplaySettings(const char* name, const char* path);
void getSequences(const char* path);
void getBarTitles(const char* path);
#endif
