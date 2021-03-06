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

#include "main.h"
#include "client.h"
#include "server.h"
#include "config.h"

// Terminate daemons
volatile sig_atomic_t term = 0;

int main(int argc, const char *argv[])
{
    // Set Flags if some are set
    handleFlags(argc, argv);
    return -1;
}

void handleFlags(int argc, const char *argv[]) {

    // Handle CLI flags
    static struct option long_options[] =
    {
        {"verbose", no_argument, &verbose_flag, 1},
        {"help", no_argument, 0, 'h'},
        {"now", no_argument, &now_flag, 1},
        {"config", required_argument, 0, 'f'},
        {"client", required_argument, 0, 'C'},
        {"server", required_argument, 0, 'S'},
        {0, 0, 0, 0}
    };

    int c;
    const char *helptext="--verbose, -v:                  echoes every value to stdout \n\
    --config CFG_File, -f CFG_File: expects path to Config file\n\
    --client -C: [start|stop|restart|update|delete] : control client\n\
    --server -S: [start|stop|restart] : control server\n";

    cliType opt;
    while ((c = getopt_long(argc, argv, "C:f:hnS:v", long_options, NULL)) != -1) {
        switch (c) {
        case 'v':
            verbose_flag = 1;
            break;
        case 'h':
            fprintf(stdout, "%s", helptext);
            exit(0);
            break;
        case 'n':
            now_flag = 1;
            opt = CSTART;
            break;
        case 'C':
            if (!strcmp(optarg, "start")) {
                opt = CSTART;
                break;
            }
            if (!strcmp(optarg, "stop")) {
                opt = CSTOP;
                break;
            }
            if (!strcmp(optarg, "restart")) {
                opt = CRESTART;
                break;
            }
            if (!strcmp(optarg, "delete")) {
                opt = CDELETE;
                break;
            }
            if (!strcmp(optarg, "update")) {
                opt = CUPDATE;
                break;
            } else {
                fprintf(stderr, "%s not a valid option for --client\n", optarg);
                exit(-1);
            }
            break;
        case 'S':
            if (!strcmp(optarg, "start")) {
                opt = SSTART;
                break;
            }
            if (!strcmp(optarg, "stop")) {
                opt = SSTOP;
                break;
            }
            if (!strcmp(optarg, "restart")) {
                opt = SRESTART;
                break;
            } else {
                fprintf(stderr, "%s not a valid option for --client\n", optarg);
                exit(-1);
            }
            break;
        case 'f':
            setLocation(optarg);
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
    switch (opt) {
    case CSTART:
        client(START);
        exit(0);
        break;
    case CSTOP:
        daemon_stop('c');
        exit(0);
        break;
    case CRESTART:
        daemon_stop('c');
        client(START);
        exit(0);
        break;
    case CDELETE:
        client(DEL);
        exit(0);
        break;
    case CUPDATE:
        client(UPDT);
        exit(0);
        break;
    case SSTART:
        server();
        exit(0);
        break;
    case SSTOP:
        daemon_stop('s');
        exit(0);
        break;
    case SRESTART:
        daemon_stop('s');
        server();
        exit(0);
        break;
    default:
        break;
    }
}

struct pidfh* daemon_start(char who) {
    char pidp[28];
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
    char pidp[28];
    sprintf(pidp, "/var/run/fidistat-%c.pid", who);

    struct pidfh *pfh;
    pid_t otherpid;

    pfh = pidfile_open(pidp, 0600, &otherpid);
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

// Return full path to json/csv
char* composeFileName(const char* prefix, const char* name, const char* type) {
    char file[strlen(getPath())+strlen(prefix)+strlen(name)+strlen(type)+3];
    sprintf(file, "%s%s-%s.%s",getPath(), prefix, name, type);
    char* retFile = strdup(file);
    return retFile;
}

// Delete File
void delete(const char *client, const char *name) {
    char* file = composeFileName(client, name, "json");
    if (remove(file) == -1) {
        file = composeFileName(client, name, "csv");
        remove(file);
    }

}

void debug(Status *stat) {
    if (stat->type != 2) {
        int i;
        for (i = 0; i < stat->num; i++) {
            syslog(LOG_INFO, "Result %i of %s: %f\n", i, stat->name, stat->result[i]);
        }
    }
}
