#include <jansson.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "main.h"
#include "bootstrap.h"
#include "config.h"

json_t *root, *graph, *sequences;
json_error_t error;

void bootstrap(Status* status) {
    char file[strlen(path)+strlen(status->name)+6];
    sprintf(file, "%s%s.json",path, status->name);
    if ( access( file, F_OK ) == -1 ) {
        if(verbose_flag) {  
            printf("%s not found, creating new File", file);
        }
        createFile(status);
    }
}

void createFile(Status* status) {
    root = json_object();
    graph = json_object();
    sequences = json_array();

    char confPath[strlen(status->name)+10];
    sprintf(confPath, "%s.display" , status->name);
    
    // Load Display Settings: title, etc
    getDisplaySettings(confPath, "graph");

    // Create Datasequences
    sprintf(confPath, "%s.sequencetitles" , status->name);
    getSequences(confPath);
    json_object_set(graph, "datasequences", sequences);
    json_object_set(root, "graph", graph);

    // Print created JSON
    char file[strlen(confPath)+strlen(status->name)+6];
    sprintf(file, "%s%s.json",path, status->name);
    json_dump_file(root, file, JSON_INDENT(4)|JSON_PRESERVE_ORDER);
}

void addNewSubSetting(const char* subObj) {
    json_object_set(graph, subObj, json_object());
}

void addNewString(const char* key, const char* value, const char* subObj) {
    if (!strcmp(subObj,  "graph")) {
        json_object_set(graph, key, json_string(value)); 
    } else {
        json_object_set(json_object_get(graph, subObj), key, json_string(value)); 
    }
}

void addNewInt(const char* key, int value, const char* subObj) {
    if (!strcmp(subObj, "graph")) {
        json_object_set(graph, key, json_integer(value)); 
    } else {
        json_object_set(json_object_get(graph, subObj), key, json_integer(value)); 
    }
}

void addNewSequence(const char* title) {
    json_t* newSeq = json_object();
    json_object_set(newSeq, "title", json_string(title));
    json_object_set(newSeq, "datapoints", json_array());
    json_array_append(sequences, newSeq);
}
