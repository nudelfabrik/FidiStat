#ifndef BOOTSTRAP
#define BOOTSTRAP

#include "main.h"
#include <jansson.h>

void bootstrap(Status* status);
void createFile(Status* status);
int checkForBootstrap(const char* name);

json_t* getDisplaySettings(const char* name, const char* subSetting);

json_t* getSequences(const char* name);

void getBarTitles(json_t* sequences_j, const char* name);

#endif
