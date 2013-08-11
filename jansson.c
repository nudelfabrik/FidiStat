/*
 * Written by Benedikt Scholtes
 * bene.scholtes@gmail.com
 * 2013
 */

#include <jansson.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "jansson.h"

struct statStruct stats[10];

void makeJansson(int i) {
    json_t *root, *dataseq, *arry, *newval;
    json_error_t error;
    char *test;

    char file[OUTPUT_SIZE];
    sprintf(file, "%s.json", stats[i].name);
    root = json_load_file(file, 0, &error);
    if (!root) {
    printf("Unable to load parmaters! error: on line %d: %s\n", error.line, error.text); 
    }
    dataseq = json_object_get(json_object_get(root, "graph"), "datasequences");
    int j;
    for (j = 0; j < json_array_size(dataseq); j++) {
        arry = json_object_get(json_array_get(dataseq, j),"datapoints"); 
        if (json_array_size(arry) >= MAXCOUNT) {
             json_array_remove(arry,0);
        }
        newval = json_pack("{sssf}", "title", zeit, "value", stats[i].result[j]);
        json_array_append_new(arry, newval);
    }
    json_dump_file(root, file, JSON_PRESERVE_ORDER || JSON_INDENT(2));
}
