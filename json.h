#ifndef JSON
#define JSON
#include "main.h"
#include <jansson.h>

json_t* makeJansson(Status *Stat);
void makeCSV(Status *Stat);
json_t* makeStat(Status *Stat);
void sendJSON(json_t *array, const char *name);
int  pasteJSON(json_t *array, const char *clientName);
void dumpJSON(json_t *root, const char *name);
json_t* getDataSequences(json_t* graph);
json_t* getSingleSeqeunce(json_t* sequences, int i);
const char* getTitle(json_t* root);
const char* getType(json_t* root);
int check(json_t* object);
void printError(const char* name);


#endif
