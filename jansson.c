#include <jansson.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "jansson.h"


void makeJansson(Status *stat) {
    json_t *root, *dataseq, *arry, *newval;
    json_error_t error;

    char file[OUTPUT_SIZE];
    sprintf(file, "%s%s.json",path, stat->name);
    root = json_load_file(file, 0, &error);
    if (!root) {
        printf("Unable to load parmaters! error: on line %d: %s\n", error.line, error.text); 
        exit(1);
    }
    dataseq = json_object_get(json_object_get(root, "graph"), "datasequences");
    int j;
    for (j = 0; j < json_array_size(dataseq); j++) {
        arry = json_object_get(json_array_get(dataseq, j),"datapoints"); 
        if (stat->type == 0) {
            if (json_array_size(arry) >= MAXCOUNT) {
                 json_array_remove(arry,0);
            }
            newval = json_pack("{sssf}", "title", zeit, "value", stat->result[j]);
            json_array_append_new(arry, newval);
        } else {
            int k;
            for (k = 0; k < sizeof(stat->result); k++) {
                json_real_set(json_object_get(json_array_get(arry, k), "value"), stat->result[k]); 
            }
        }
    }
        json_dump_file(root, file, JSON_PRESERVE_ORDER || JSON_INDENT(2));
}

void makeCSV(Status *stat) {
    FILE *fp;
    char file[OUTPUT_SIZE];
    char output[OUTPUT_SIZE];
    sprintf(file, "%s%s.csv",path, stat->name);
    fp = fopen(file, "w");
    strcat(output, stat->regex);
    strcat(output, "\n");
    strcat(output, stat->raw);
    fprintf(fp, "%s",output); 
}
