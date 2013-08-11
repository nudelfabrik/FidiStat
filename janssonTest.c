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

#define PATH    "~/"
int main(int argc, const char *argv[])
{
    json_t *root;
    json_error_t error;

    root = json_load_file("load.json", 0, &error);

    if(!json_is_object(root))
    {
            fprintf(stderr, "error: root is not an array\n");
                return 1;
    }


}
