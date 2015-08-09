#include <jansson.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include "client.h"
#include "bootstrap.h"
#include "config.h"
#include "json.h"

json_t *root, *graph, *sequences;
json_error_t error;

// Check if File exist and  create if needed
void bootstrap(Status* status) {
    if (status->type != 2) {
        if (checkForBootstrap(status->name)) {
            if(verbose_flag) {  
                syslog(LOG_INFO, "%s.json not found, creating new File", status->name);
            }
            createFile(status);
        }
    }
}

int checkForBootstrap(const char* name) {
    if (local) {
        char file[strlen(path)+strlen(name)+6];
        sprintf(file, "%s%s.json",path, status->name);
        return (access( file, F_OK ) == -1);
    } else {
        // Ask Server, if bootstrap is needed
    }

}
// Create new JSON File
void createFile(Status* status) {
    if (status->type == 2) {
        return;
    }
    root = json_object();
    graph = json_object();
    sequences = json_array();

    getDisplaySettings(status->name, "graph");

    // Create Datasequences
    getSequences(status->name);

    // If Type is Bar, load Bartitles
    if (status->type == 1) {
        getBarTitles(status->name);
    }

    // Build json Object
    json_object_set(graph, "datasequences", sequences);
    json_object_set(root, "graph", graph);

    // Print created JSON
    sendJSON(root, status->name);
}

// Adds new SubObject under "Graph"
void addNewSubSetting(const char* subObj) {
    json_object_set(graph, subObj, json_object());
}

// Adds new String Key/Value pair to Graph or a SubObject of Graph
void addNewString(const char* key, const char* value, const char* subObj) {
    if (!strcmp(subObj,  "graph")) {
        json_object_set(graph, key, json_string(value)); 
    } else {
        json_object_set(json_object_get(graph, subObj), key, json_string(value)); 
    }
}

// Adds new Integer Key/Value pair to Graph or a SubObject of Graph
void addNewInt(const char* key, int value, const char* subObj) {
    if (!strcmp(subObj, "graph")) {
        json_object_set(graph, key, json_integer(value)); 
    } else {
        json_object_set(json_object_get(graph, subObj), key, json_integer(value)); 
    }
}

// Add new Sequence to "sequences"
void addNewSequence(const char* title, const char* colour) {
    json_t* newSeq = json_object();
    json_object_set(newSeq, "title", json_string(title));
    // If Colour is specified, set colour
    if (colour) {
        json_object_set(newSeq, "color", json_string(colour));
    }
    json_object_set(newSeq, "datapoints", json_array());
    json_array_append(sequences, newSeq);
}

// Add new Title/value Object to datapoints for Bar Graph
void addNewBarTitle(const char* title) {
    json_t* newTitle = json_object();
    json_object_set(newTitle, "title", json_string(title));
    json_object_set(newTitle, "value", json_real(0));
    json_array_append(json_object_get(json_array_get(sequences, 0), "datapoints"), newTitle);
}
