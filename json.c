#include <jansson.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include "client.h"
#include "json.h"

int makeJansson(Status *stat) {
    json_t *newval, *values;
    values = json_array();

    int j;
    for (j = 0; j < 10/* TODO Sizeof Results */; j++) {
        if (stat->type == 0) {
            newval = json_pack("{sssf}", "title", zeit, "value", stat->result[j]);
        } else {
            json_real_set(newval, stat->result[j]);
        }
        if (!newval) {
            syslog(LOG_ERR, "error in creating new entry for %s.json\n", stat->name);
            return 0;
        }
        json_array_append_new(values, newval);
    }
    // send json
    if (!dry_flag) {
        sendJSON(values, stat->name);
    }
    return 1;
        
}

int pasteJSON(json_t *new, const char *name) {
    json_t *root, *dataseq, *graph, *arry, *newval;
    json_error_t error;

    char file[strlen(path)+strlen(name)+6];

    sprintf(file, "%s%s.json",path, stat->name);
    // Load *.json
    root = json_load_file(file, 0, &error);
    // CHeck for Errors
    if (!root) {
        syslog(LOG_ERR, "Unable to load json File! error: on line %d: %s\n", error.line, error.text); 
        exit(1);
    }
    // Get old Data
    graph = json_object_get(root, "graph");
    dataseq = getDataSequences(graph);
    if (!check(dataseq)) {
        printError(name);
    }
    // Process Every Datasequence
    int j;
    for (j = 0; j < json_array_size(dataseq); j++) {
        // If Type is 0 /Line (standard case) append new value at the bottom 
        arry = getSingleSeqeunce(dataseq, j);
        if (strcmp(getType(root), "line")) {
            if (json_array_size(arry) >= maxCount) {
                 if (json_array_remove(arry,0)) {
                     syslog(LOG_ERR, "error in processing %s.json\n", name);
                     return 0;
                 }
            }
            if (json_array_append_new(arry, json_array_get(new, j))) {
                syslog(LOG_ERR, "error in appending new entry in %s.json\n", name);
                return 0;
            }
        // When Type is Bar, every Entry has its own name and you change the value
        } else {
            int k;
            for (k = 0; k < json_array_size(arry); k++) {
                if (json_real_set(json_object_get(json_array_get(arry, k), "value"), json_real_value(json_array_get(new, k))) ) {
                    return 0;
                }
                    syslog(LOG_ERR, "error in changing entry in %s.json\n", name);
            }
        }
    }

}


void sendJSON(json_t *root, const char *name) {
    if (local) {
        dumpJSON(root, name);
    } else {
        // Send JSON to Server
    }
}

void dumpJSON(json_t *root, const char *name) {
    char file[strlen(path)+strlen(name)+6];
    sprintf(file, "%s%s.json",path, name);
    if (json_dump_file(root, file, JSON_PRESERVE_ORDER | JSON_INDENT(2))) {
        syslog(LOG_ERR, "error in writing back to %s.json", name);
    }
    syslog(LOG_DEBUG, "written to %s.json", name);
}

json_t* getDataSequences(json_t* graph) {
    return json_object_get(graph, "datasequences");
}

const char* getTitle(json_t* root) {
    return json_string_value(json_object_get(json_object_get(root, "graph"), "title"));
}

const char* getType(json_t* root) {
    return json_string_value(json_object_get(json_object_get(root, "graph"), "type"));
}

json_t* getSingleSeqeunce(json_t* sequences, int i) {
        return json_object_get(json_array_get(sequences, i),"datapoints"); 
}

int check(json_t* object) {

        if (!object) {
            return 0;
        } else {
            return 1;
        }
}

void makeStat(Status *stat) {
    if (stat->type == 2) {
        if (verbose_flag) {
            debug(stat);
        }
        makeCSV(stat);
    } else {
        if (verbose_flag) {
            debug(stat);
        }
        makeJansson(stat);
    }
}

void printError(const char* name) {
    syslog(LOG_ERR, "Can't get data from %s.json\n", name);
}
// If type is 2, create a new .csv evertime the command runs
void makeCSV(Status *stat) {
    FILE *fp;
    char file[OUTPUT_SIZE];
    char output[OUTPUT_SIZE] = "";
    strcat(output, stat->csv);
    strcat(output, "\n");

    char raw[OUTPUT_SIZE] = "";
    FILE *cmd;
    cmd = popen(stat->cmmd, "r");  
    if (fgets(raw, sizeof(*raw-1), cmd) == NULL) {
        syslog(LOG_ERR, "Error executing command %s\n", stat->name);
    }
    if (pclose(cmd) != 0) {
        syslog(LOG_ERR, "Command of %s exits != 0\n", stat->name);
    }
    strcat(output, raw);

    if (!dry_flag) {
        sprintf(file, "%s%s.csv",path, stat->name);
        fp = fopen(file, "w");
        fprintf(fp, "%s",output); 
        fclose(fp);
    }
}
