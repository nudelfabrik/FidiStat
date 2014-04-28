#include <jansson.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "jansson.h"


void makeJansson(Status *stat) {
    json_t *root, *dataseq, *arry, *newval;
    json_error_t error;

    char file[strlen(path)+strlen(stat->name)+6];
    sprintf(file, "%s%s.json",path, stat->name);
    //Load *.json
    root = json_load_file(file, 0, &error);
    //CHeck for Errors
    if (!root) {
        printf("Unable to load parmaters! error: on line %d: %s\n", error.line, error.text); 
        printf("File: %s\n", stat->name);
        exit(1);
    }
    //Get old Data
    dataseq = json_object_get(json_object_get(root, "graph"), "datasequences");
    int j;
    for (j = 0; j < json_array_size(dataseq); j++) {
        arry = json_object_get(json_array_get(dataseq, j),"datapoints"); 
    //If Type is 0 /Line (standard case) append new value at the bottom 
        if (stat->type == 0) {
            if (json_array_size(arry) >= MAXCOUNT) {
                 json_array_remove(arry,0);
            }
            newval = json_pack("{sssf}", "title", zeit, "value", stat->result[j]);
            json_array_append_new(arry, newval);
    //When Type is Bar, every Entry has its own name and you change the value
        } else {
            int k;
            for (k = 0; k < sizeof(stat->result); k++) {
                json_real_set(json_object_get(json_array_get(arry, k), "value"), stat->result[k]); 
            }
        }
    }
    //write modified json
    json_dump_file(root, file, JSON_PRESERVE_ORDER || JSON_INDENT(2));
}

//If type is 2, create a new .csv evertime the command runs
void makeCSV(Status *stat) {
    FILE *fp;
    char file[OUTPUT_SIZE];
    char output[OUTPUT_SIZE] = "";
    sprintf(file, "%s%s.csv",path, stat->name);
    fp = fopen(file, "w");
    strcat(output, stat->regex);
    strcat(output, "\n");
    strcat(output, stat->raw);
    fprintf(fp, "%s",output); 
}
