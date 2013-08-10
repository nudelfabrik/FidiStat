/*
 * Written by Benedikt Scholtes
 * bene.scholtes@gmail.com
 * 2013
 */

#include <jansson.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <libconfig.h>
#include "regex.h"
#include "config.h"
#include "main.h"

struct statStruct stats[10];

int main(int argc, const char *argv[])
{
    FILE *fp;
    char source[1024];
    char *srcPnt = source;
    
    initConf();
    int statNum = getStatNum(); 
 
    //import the config file
    int i = 0;
    for (i = 0; i < statNum; i++) {
        getConfList(i);
        getConfEnable(i);
        if (stats[i].enabled) {
            getConfRegex(i);
            getConfCmmd(i);
        }
    }

    for (i = 0; i < statNum; i++) {
        if (stats[i].enabled) {
            fp = popen(stats[i].cmmd, "r");  
            fgets(source, sizeof(source)-1, fp);
            regexing(stats[i].regex, srcPnt);
            printf("%s", source);
        }
    }
    destroyConf(); 
    return 0;
}
