#ifndef JSON
#define JSON
#include "main.h"
#include <jansson.h>

int makeJansson(Status *Stat);
void makeCSV(Status *Stat);
void makeStat(Status *Stat);
void dumpJSON(json_t *root, const char *name);
void sendJSON(json_t *root, const char *name);
json_t* getDataSequences(json_t* graph);
json_t* getSingleSeqeunce(json_t* sequences, int i);
const char* getTitle(json_t* root);
const char* getType(json_t* root);
int check(json_t* object);
void printError(const char* name);


#endif
