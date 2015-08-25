#include <unistd.h>
#include "client.h"
#include "bootstrap.h"
#include "config.h"
#include "json.h"
#include "tls.h"

json_error_t error;

// Check if File exist and  create if needed
void bootstrap(Status* status) {
    if (checkForBootstrap(status->name)) {
        if(verbose_flag) {  
            syslog(LOG_INFO, "%s.json not found, creating new File", status->name);
        }
        createFile(status, CREATE);
    }
}

int checkForBootstrap(const char* name) {
    char* file = composeFileName(getClientName(), name, "json");
    return (access( file, F_OK ) == -1);
}

// --------------------
// Create new JSON File
// --------------------
void createFile(Status* status, int type) {
    if (status->type == 2) {
        return;
    }
    json_t *root = json_object();

    json_t *graph = getDisplaySettings(status->id, "graph");

    // Create Datasequences
    json_t *sequences = getSequences(status->id);

    // If Type is Bar, load Bartitles
    if (status->type == 1) {
        getBarTitles(sequences, status->id);
    }

    // Build json Object
    json_object_set(graph, "datasequences", sequences);
    json_object_set(root, "graph", graph);

    // Print created JSON
    char* file = composeFileName(getClientName(), status->name, "json");

    // Send/dump created .json
    if (getLocal()) {
        dumpJSON(root, file);
    } else {
        // Type either CREATE or UPDATE
        struct tls* ctx = initCon(type, 1);
        json_t* payload = json_object();
        json_object_set(payload, "name", json_string(status->name));
        json_object_set(payload, "payload", root);
        char * payloadStr = json_dumps(payload, JSON_COMPACT);
        sendOverTLS(ctx, payloadStr);
        free(payloadStr);
    }
}

//--------------------------
// Create JSON with settings
//--------------------------
json_t* getDisplaySettings(int id, const char* subSetting) {

    json_t *graph = json_object();
    char path[25+ strlen(subSetting)];

    // compose path to this Settings location
    if (!strcmp(subSetting,  "graph")) {
        sprintf(path, "settings.[%d].display" , id);
    } else {
        sprintf(path, "settings.[%d].display.%s", id, subSetting);
    }
    // Get Display Setting of name
    config_setting_t* display = getLookup(path);
    int numSettings = config_setting_length(display);

    // Add every Setting of Display to json file
    for (int i = 0; i < numSettings; i++) {
        config_setting_t* sett = config_setting_get_elem(display, i);

        // Int and String Settings
        if (config_setting_type(sett) == CONFIG_TYPE_INT) {
            json_object_set(graph, config_setting_name(sett), json_integer(config_setting_get_int(sett)));
        }
        if (config_setting_type(sett) == CONFIG_TYPE_STRING) {
            json_object_set(graph, config_setting_name(sett), json_string(config_setting_get_string(sett)));
        }

        // Object Settings
        if (config_setting_type(sett) == CONFIG_TYPE_GROUP) {
            const char* subName = config_setting_name(sett);

            // Add all other 
            json_t *subSettings = getDisplaySettings(id, subName);
            json_object_set(graph, subName, subSettings);
            
        }
    }
    return graph;
}

//-----------------------------------
// Create JSON with all Datasequences
//-----------------------------------
json_t* getSequences(int id) {
    json_t* sequences_j = json_array();
    char path[25];
    sprintf(path, "settings.[%d].sequencetitles" , id);
    config_setting_t* sequences_c = getLookup(path);

    sprintf(path, "settings.[%d].sequencecolors" , id);
    config_setting_t* colours_c = getLookup(path);

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
void getBarTitles(int id, json_t* sequences_j) {
    char path[25];
    sprintf(path, "settings.[%d].bartitles" , id);
    config_setting_t* sequences_c = getLookup( path);
    int numSeq = config_setting_length(sequences_c);

    for (int i = 0; i < numSeq; i++) {
        json_t* newTitle = json_object();
        json_object_set(newTitle, "title", json_string(config_setting_get_string_elem(sequences_c, i)));
        json_object_set(newTitle, "value", json_real(0));
        json_array_append(json_object_get(json_array_get(sequences_j, 0), "datapoints"), newTitle);
    }
}
