#ifndef BOOTSTRAP
#define BOOTSTRAP

#include "main.h"
#include <jansson.h>

void bootstrap(Status* status);
void createFile(Status* status, int type);
int checkForBootstrap(const char* name);

json_t* getDisplaySettings(int id, const char* subSetting);

json_t* getSequences(int id);

void getBarTitles(json_t* sequences_j, int id);

#endif
