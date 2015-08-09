#ifndef BOOTSTRAP
#define BOOTSTRAP

#include "main.h"

void bootstrap(Status* status);
void createFile(Status* status);
int checkForBootstrap(const char* name);

void addNewSubSetting(const char* subObj);
void addNewString(const char* key, const char* value, const char* subObj);
void addNewInt(const char* key, int value, const char* subObj);
void addNewSequence(const char* title, const char* colour);
void addNewBarTitle(const char* title);

#endif
