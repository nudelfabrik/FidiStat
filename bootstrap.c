#include <jansson.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "main.h"
#include "bootstrap.h"
#include "config.h"

json_t *root, *graph;
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

    char path[strlen(status->name)+10];
    sprintf(path, "%s.display" , status->name);
    getDisplaySettings(path, "graph");
        json_dumpf(graph, stdout, 0);

    json_object_set(root, "graph", graph);

    char file[strlen(path)+strlen(status->name)+6];
    //sprintf(file, "%s%s.json",getPath(), status->name);
    sprintf(file, "%s.json", status->name);
    json_dump_file(root, file, JSON_INDENT(4)|JSON_PRESERVE_ORDER);
}

void addNewSubSetting(const char* subObj) {
    printf("%s\n", subObj);
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
