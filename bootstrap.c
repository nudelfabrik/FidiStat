#include <jansson.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "bootstrap.h"
#include "config.h"

json_t *root, *graph;
json_error_t error;

void bootstrap(Status status) {
    char file[strlen(path)+strlen(stat->name)+6];
    sprintf(file, "%s%s.json",path, stat->name);
    if ( access( file, F_OK ) == -1 ) {
        if(verbose_flag) {  
            printf("%s not found, creating new File", file);
        }
        createFile(status);
    }
}

void createFile(status) {
    root = json_object();
    graph = json_object();

    char path[strlen(name)+10];
    sprintf(path, "%s.display" , status->name);
    getDisplaySettings(path, "graph");

    json_object_set(root, "graph", graph);

    char file[strlen(path)+strlen(stat->name)+6];
    sprintf(file, "%s%s.json",path, stat->name);
    json_dump_file(root, file, 0);
}

void addNewSubSetting(const char* subObj) {
    json_object_set(graph, subObj, json_object());
}

void addNewValue(const char* key, const char* value, const char* subObj) {
    if (subObj == "graph") {
        json_object_set(graph, key, json_string(value)); 
    } else {
        json_object_set(json_object_get(graph, subObj), key, json_string(value)); 
    }
}
void addNewValue(const char* key, int value, const char* subObj) {
    if (subObj == "graph") {
        json_object_set(graph, key, json_integer(value)); 
    } else {
        json_object_set(json_object_get(graph, subObj), key, json_string(value)); 
    }
}
