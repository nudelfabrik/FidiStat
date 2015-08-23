#ifndef BOOTSTRAP
#define BOOTSTRAP

#include "main.h"
#include <jansson.h>

void bootstrap(Status* status);
void createFile(Status* status);
int checkForBootstrap(const char* name);

json_t* getDisplaySettings(const char* name, const char* subSetting);
void addNewSubSetting(json_t* graph, const char* subObj);
void addNewString(json_t* graph, const char* key, const char* value, const char* subObj);
void addNewInt(json_t* graph, const char* key, int value, const char* subObj);

json_t* getSequences(const char* name);

void getBarTitles(json_t* sequences_j, const char* name);

#endif
