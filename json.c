#include <jansson.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "json.h"
#include "config.h"


int makeJansson(Status *stat) {
    json_t *root, *dataseq, *graph, *arry, *newval;
    json_error_t error;

    char file[strlen(path)+strlen(stat->name)+6];
    sprintf(file, "%s%s.json",path, stat->name);
    // Load *.json
    root = json_load_file(file, 0, &error);
    // CHeck for Errors
    if (!root) {
        printf("Unable to load json File! error: on line %d: %s\n", error.line, error.text); 
        printf("File: %s\n", stat->name);
        exit(1);
    }
    // Get old Data
    graph = json_object_get(root, "graph");
    dataseq = getSequences(graph);
    if (!check(dataseq)) {
        printError(stat->name);
    }
    // Process Every Datasequence
    int j;
    for (j = 0; j < json_array_size(dataseq); j++) {
        // If Type is 0 /Line (standard case) append new value at the bottom 
        arry = getSingleSeqeunce(dataseq, j);
        if (stat->type == 0) {
            if (json_array_size(arry) >= maxCount) {
                 if (json_array_remove(arry,0)) {
                     fprintf(stderr, "error in processing %s.json\n", stat->name);
                     return 0;
                 }
            }
            newval = json_pack("{sssf}", "title", zeit, "value", stat->result[j]);
            if (!newval) {
                fprintf(stderr, "error in creating new entry for %s.json\n", stat->name);
                return 0;
            }
            if (json_array_append_new(arry, newval)) {
                fprintf(stderr, "error in appending new entry in %s.json\n", stat->name);
                return 0;
            }
        // When Type is Bar, every Entry has its own name and you change the value
        } else {
            int k;
            for (k = 0; k < json_array_size(arry); k++) {
                if (json_real_set(json_object_get(json_array_get(arry, k), "value"), stat->result[k])) {
                    fprintf(stderr, "error in changing entry in %s.json\n", stat->name);
                    return 0;
                }
            }
        }
    }
    // write modified json
    if (!dry_flag) {
        if (json_dump_file(root, file, JSON_PRESERVE_ORDER | JSON_INDENT(2))) {
            fprintf(stderr, "error in writing back to %s.json", stat->name);
            return 0;
        }
    }
    return 1;
        
}

json_t* getSequences(json_t* graph) {
    return json_object_get(graph, "datasequences");
}

const char* getTitle(json_t* root) {
    return json_string_value(json_object_get(json_object_get(root, "graph"), "title"));
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

void printError(const char* name) {
    fprintf(stderr, "Can't get data from %s.json\n", name);
}
// If type is 2, create a new .csv evertime the command runs
void makeCSV(Status *stat) {
    FILE *fp;
    char file[OUTPUT_SIZE];
    char output[OUTPUT_SIZE] = "";
    strcat(output, getCSVtitle(stat));
    strcat(output, "\n");
    strcat(output, stat->raw);
    if (!dry_flag) {
        sprintf(file, "%s%s.csv",path, stat->name);
        fp = fopen(file, "w");
        fprintf(fp, "%s",output); 
        fclose(fp);
    }
}