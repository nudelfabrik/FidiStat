#ifndef JSON
#define JSON
#include "main.h"
#include <jansson.h>

int makeJansson(Status *Stat);
void makeCSV(Status *Stat);
json_t* getDataSequences(json_t* graph);
json_t* getSingleSeqeunce(json_t* sequences, int i);
const char* getTitle(json_t* graph);
int check(json_t* object);
void printError(const char* name);


#endif
