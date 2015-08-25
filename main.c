/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Benedikt Scholtes
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <sys/types.h>
#include <sys/wait.h>
#include "main.h"
#include "client.h"
#include "server.h"
#include "config.h"

volatile sig_atomic_t term = 0;

void handleChild(int sig) {
    wait(NULL);
}

void handleSigterm(int sig) {
    term = 1;
}

int main(int argc, const char *argv[])
{
    // Set Flags if some are set
    handleFlags(argc, argv);
    return -1;
}

struct *pidfh daemon_start(char who) {
    char*  pidp[28];
    sprintf(pidp, "/var/run/fidistat-%c.pid", who);
    struct pidfh *pfh;
    pid_t otherpid;

    pfh = pidfile_open(pidp, 0600, &otherpid);
    if (pfh == NULL) {
        if (errno == EEXIST) {
            fprintf(stdout, "Daemon already running with PID %d\n", otherpid);
            exit(-1);
        }
            fprintf(stdout, "Cannot create pidfile\n");
    }   

    if  (daemon(0, 0) == -1) {
        fprintf(stderr, "Cannot daemonize");
        pidfile_remove(pfh);
        exit(-1);
    }

    pidfile_write(pfh);
    return pfh;
}

void daemon_stop(char who) {
    struct pidfh *pfh;
    pid_t otherpid;

    pfh = pidfile_open("/var/run/fidistat.pid", 0600, &otherpid);
    if (pfh == NULL) {
        if (errno == EEXIST) {
            if (!kill(otherpid, SIGTERM)) {
                fprintf(stdout, "Stopped FidiStat(%d) successfully\n", otherpid);
                return;
            } else {
                switch(errno) {
                    case EPERM:
                        fprintf(stderr, "Insufficient rights.\n");
                        exit(-1);
                        break;
                    case ESRCH:
                        fprintf(stderr, "PID not found, removing pidfile\n");
                        pidfile_remove(pfh);
                        exit(-1);
                        break;
                    default:
                        exit(-1);
                        break;
                }
            }
        }
    }   
    pidfile_remove(pfh);
    fprintf(stdout, "FidiStat not running?\n");
    exit(-1);

}

void handleFlags(int argc, const char *argv[]) {

    // Handle CLI flags
    static struct option long_options[] =
    {
        {"verbose", no_argument, &verbose_flag, 1},
        {"dry", no_argument, &dry_flag, 1},
        {"help", no_argument, 0, 'h'},
        {"clean", no_argument, &clean_flag, 1},
        {"now", no_argument, &now_flag, 1},
        {"delete", no_argument, &delete_flag, 1},
        {"config", required_argument, 0, 'f'},
        {0, 0, 0, 0}
    };

    int c;
    const char *helptext="--verbose, -v:                  echoes every value to stdout \n\
    --dry, -d:                      Dry run, nothing is written to disk\n\
    --config CFG_File, -f CFG_File: expects path to Config file\n\
    --clean -c:                     remove all files not enabled\n\
    --delete -x:                    remove all files\n"; 

    while ((c = getopt_long(argc, argv, "cdfhnvx", long_options, NULL)) != -1) {
        switch (c) {
        case 'v':
            verbose_flag = 1;
            break;
        case 'h':
            fprintf(stdout, "%s", helptext);
            exit(0);
            break;
        case 'd':
            dry_flag = 1;
            break;
        case 'n':
            now_flag = 1;
            break;
        case 'f':
            setLocation(optarg);
            break;
        case 'c':
            clean_flag = 1;
            break;
        case 'x':
            delete_flag = 1;
            break;
        case 0:
            break;
        case '?':
        default:     
        fprintf(stderr, "%s: option `-%c' is invalid: ignored\n",
            argv[0], optopt);
            break;
        }
    }

    // Handle Daemon Controls
    int i;
    for (i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "client")) {
            if (!strcmp(argv[i+1], "start")) {
                client(START);
                exit(0);
            }
            if (!strcmp(argv[i+1], "stop")) {
                daemon_stop('c');
                exit(0);
            }
            if (!strcmp(argv[i+1], "restart")) {
                daemon_stop('c');
                client(START);
                exit(0);
            }
        } else if (!strcmp(argv[i], "server")) {
            if (!strcmp(argv[i+1], "start")) {
                server();
                exit(0);
            }
            if (!strcmp(argv[i+1], "stop")) {
                daemon_stop('s');
                exit(0);
            }
            if (!strcmp(argv[i+1], "restart")) {
                daemon_stop('s');
                server();
                exit(0);
            }
        }
    }
}

char* composeFileName(const char* prefix, const char* name, const char* type) {
    char file[strlen(getPath())+strlen(prefix)+strlen(name)+strlen(type)+3];
    sprintf(file, "%s%s-%s.%s",getPath(), prefix, name, type);
    char* retFile = strdup(file);
    return retFile;
}

void debug(Status *stat) {
    //syslog(LOG_INFO, "\nOutput of %s:\n%s\n", stat->name, stat->raw);
    if (stat->type != 2) {
        int i;
        for (i = 0; i < stat->num; i++) {
            syslog(LOG_INFO, "Result %i of %s: %f\n", i, stat->name, stat->result[i]);
        }
    }
}
