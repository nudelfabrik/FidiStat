#include <jansson.h>
#include <libconfig.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include "client.h"
#include "bootstrap.h"
#include "config.h"
#include "json.h"

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
        char file[strlen(path)+strlen(name)+strlen(clientName)+6];
        sprintf(file, "%s%s-%s.json",path, clientName, name);
        return (access( file, F_OK ) == -1);
    } else {
        // Ask Server, if bootstrap is needed
        return 0;
    }

}

// --------------------
// Create new JSON File
// --------------------
void createFile(Status* status) {
    if (status->type == 2) {
        return;
    }
    json_t *root = json_object();

    json_t *graph = getDisplaySettings(status->name, "graph");

    // Create Datasequences
    json_t *sequences = getSequences(status->name);

    // If Type is Bar, load Bartitles
    if (status->type == 1) {
        getBarTitles(sequences, status->name);
    }

    // Build json Object
    json_object_set(graph, "datasequences", sequences);
    json_object_set(root, "graph", graph);

    // Print created JSON
    char file[strlen(path)+ strlen(clientName)+strlen(status->name)+6];
    sprintf(file, "%s%s-%s.json",path, clientName, status->name);

    dumpJSON(root, file);
}

//--------------------------
// Create JSON with settings
//--------------------------
json_t* getDisplaySettings(const char* name, const char* subSetting) {

    json_t *graph = json_object();
    char path[strlen(name)+20];
    if (!strcmp(subSetting,  "graph")) {
        sprintf(path, "%s.display" , name);
    } else {
        sprintf(path, "%s.display.%s", name, subSetting);
    }
    // Get Display Setting of name
    config_setting_t* display = config_lookup(&config, path);
    int numSettings = config_setting_length(display);

    // Add every Setting of Display to json file
    for (int i = 0; i < numSettings; i++) {
        config_setting_t* sett = config_setting_get_elem(display, i);

        // Int and String Settings
        if (config_setting_type(sett) == CONFIG_TYPE_INT) {
            addNewInt(graph, config_setting_name(sett),config_setting_get_int(sett), subSetting);
        }
        if (config_setting_type(sett) == CONFIG_TYPE_STRING) {
            addNewString(graph, config_setting_name(sett),config_setting_get_string(sett), subSetting);
        }

        // Object Settings
        if (config_setting_type(sett) == CONFIG_TYPE_GROUP) {
            const char* subName = config_setting_name(sett);
            addNewSubSetting(graph, subName);

            // Add all other 
            getDisplaySettings(name, subName);
        }
    }
    return graph;
}

// Adds new SubObject under "Graph"
void addNewSubSetting(json_t* graph, const char* subObj) {
    json_object_set(graph, subObj, json_object());
}

// Adds new String Key/Value pair to Graph or a SubObject of Graph
void addNewString(json_t* graph, const char* key, const char* value, const char* subObj) {
    if (!strcmp(subObj,  "graph")) {
        json_object_set(graph, key, json_string(value)); 
    } else {
        json_object_set(json_object_get(graph, subObj), key, json_string(value)); 
    }
}

// Adds new Integer Key/Value pair to Graph or a SubObject of Graph
void addNewInt(json_t* graph, const char* key, int value, const char* subObj) {
    if (!strcmp(subObj, "graph")) {
        json_object_set(graph, key, json_integer(value)); 
    } else {
        json_object_set(json_object_get(graph, subObj), key, json_integer(value)); 
    }
}

//-----------------------------------
// Create JSON with all Datasequences
//-----------------------------------
json_t* getSequences(const char* name) {
    json_t* sequences_j = json_array();
    char path[strlen(name)+20];
    sprintf(path, "%s.sequencetitles" , name);
    config_setting_t* sequences_c = config_lookup(&config, path);

    sprintf(path, "%s.sequencecolors" , name);
    config_setting_t* colours_c = config_lookup(&config, path);

    int numSeq = config_setting_length(sequences_c);

    // Add every Setting of Display to json file
    for (int i = 0; i < numSeq; i++) {
        json_t* newSeq_j = json_object();
        json_object_set(newSeq_j, "title", json_string(config_setting_get_string_elem(sequences_c, i)));
        if (colours_c) {
            json_object_set(newSeq_j, "color", json_string(config_setting_get_string_elem(colours_c, i)));
        }
        json_object_set(newSeq_j, "datapoints", json_array());
        json_array_append(sequences_j, newSeq_j);
    }
    return sequences_j;
}


//------------------
// Create Bar titles
//------------------
//
void getBarTitles(json_t* sequences_j, const char* name) {
    char path[strlen(name)+20];
    sprintf(path, "%s.bartitles" , name);
    config_setting_t* sequences_c = config_lookup(&config, path);
    int numSeq = config_setting_length(sequences_c);

    for (int i = 0; i < numSeq; i++) {
        json_t* newTitle = json_object();
        json_object_set(newTitle, "title", json_string(config_setting_get_string_elem(sequences_c, i)));
        json_object_set(newTitle, "value", json_real(0));
        json_array_append(json_object_get(json_array_get(sequences_j, 0), "datapoints"), newTitle);
    }
}
