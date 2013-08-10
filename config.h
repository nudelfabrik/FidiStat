#ifndef CONFIG
#define CONFIG
void initConf();
void destroyConf();
const char * getConfList(int i);
int getStatNum();
void getConfEnable(int i);
void getConfCmmd(int i);
void getConfRegex(int i);

#endif
