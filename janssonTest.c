/*
 * Written by Benedikt Scholtes
 * bene.scholtes@gmail.com
 * 2013
 */

#include <jansson.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <regex.h>

//uptime:"( [0-9]?[0-9],[0-9][0-9]){3,3}"
#define PATH    "~/"
int main(int argc, const char *argv[])
{
    regex_t regex;
    int retex;
    FILE *fp;
    int status;
    char source[1024];
char * path = "0:36  up 23 days,  9:51, 3 users, load averages: 0,85 0,59 0,53";
const char *srcpnt = source;
    size_t maxGroups = 3;
    regmatch_t pmatch;

    //char *cmmd = malloc(256);
    //sprintf(cmmd, "uptime %s", PATH);
    fp = popen("uptime", "r");
    //free(cmmd);
    retex = regcomp( &regex, "[0-9]?[0-9],[0-9][0-9]", REG_EXTENDED);
    //if( retex ){ fprintf(stderr, "Could not compile regex\n"); exit(1); }
    char res [1024];
            
            
    while (fgets(source, sizeof(source)-1, fp) != NULL) {
        printf("%s",source);
          while (regexec(&regex, srcpnt, 1, &pmatch, 0) == 0) {
                sprintf(res,"%.*s\n", (int)(pmatch.rm_eo - pmatch.rm_so), &srcpnt[pmatch.rm_so]);
                srcpnt += pmatch.rm_eo;
                printf("%s", res);
           }
        
        
    }    
    
    return 0;
}
