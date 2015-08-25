#ifndef JSON
#define JSON
#include "main.h"
#include <jansson.h>

// create payloads
json_t* makeStat(Status *Stat);
json_t* makeJansson(Status *Stat);
json_t* makeCSV(Status *Stat);

// paste payload into existing .json
int  pasteJSON(json_t *array, const char *clientName);

// get file.json, migrate datapoints to new and write that back to disk.
void mergeJSON(json_t *root, const char *name);

// Write json_t to file
void dumpJSON(json_t *root, const char *name);

json_t* getDataSequences(json_t* graph);
json_t* getSingleSeqeunce(json_t* sequences, int i);
const char* getTitle(json_t* root);
const char* getType(json_t* root);
int check(json_t* object);
void printError(const char* name);


#endif
