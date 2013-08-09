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
    size_t maxGroups = 3;
    regmatch_t pmatch[maxGroups];

    //char *cmmd = malloc(256);
    //sprintf(cmmd, "uptime %s", PATH);
    fp = popen("uptime", "r");
    //free(cmmd);
    retex = regcomp( &regex, "( [0-9]?[0-9],[0-9][0-9]){3,3}", REG_EXTENDED);
    //if( retex ){ fprintf(stderr, "Could not compile regex\n"); exit(1); }
            
            
    while (fgets(source, sizeof(source)-1, fp) != NULL) {
          if (regexec(&regex, source, maxGroups, pmatch, 0) == 0) {
                if (pmatch[0].rm_so == (size_t)-1)
                    break;  // No more groups
                char sourceCopy[strlen(source) + 1];
                strcpy(sourceCopy, source);
                sourceCopy[pmatch[0].rm_eo] = 0;
                printf("%s",
                sourceCopy + pmatch[0].rm_so);
           }
        
        
    }    
    
    return 0;
}
