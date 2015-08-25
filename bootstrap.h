#ifndef BOOTSTRAP
#define BOOTSTRAP

#include "main.h"
#include <jansson.h>

// check if local file needs bootstrapping
void bootstrap(Status* status);

// create full JSON object and write/send
void createFile(Status* status, int type);
int checkForBootstrap(const char* name);

// Extract options from the config file
json_t* getDisplaySettings(int id, const char* subSetting);
json_t* getSequences(int id);
void getBarTitles(int id,json_t* sequences_j);

#endif
