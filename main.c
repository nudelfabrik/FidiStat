/*
 * Written by Benedikt Scholtes
 * bene.scholtes@gmail.com
 * 2013
 */

#include <jansson.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "regex.h"

//uptime:"( [0-9]?[0-9],[0-9][0-9]){3,3}"
#define PATH    "~/"

int main(int argc, const char *argv[])
{
    FILE *fp;
    int status;
    char source[1024];
    char *srcPnt = source;

    //char *cmmd = malloc(256);
    //sprintf(cmmd, "uptime %s", PATH);
    fp = popen("uptime", "r");
    //free(cmmd);
    fgets(source, sizeof(source)-1, fp);
    regexing("( [0-9]?[0-9],[0-9][0-9]){3,3}", srcPnt);
            
    printf("%s", source);
    
    return 0;
}
