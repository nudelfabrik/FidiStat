#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/event.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <unistd.h>
#include <jansson.h>
#include "server.h"
#include "json.h"
#include "config.h"
#include "client.h"
#include "tls.h"
#include "main.h"


#define MAXSOCK 5
#define MAXCONCUR 16
// signal Handler
void handleSigterm_S(int sig) {
    if (sig == SIGTERM) {
        term = 1;
    }
}

void server() {
    // load Config File and Settings
    fprintf(stdout, "Starting fidistat Server...\n");
    openlog("fidistat-server", LOG_PID, LOG_DAEMON);
    syslog(LOG_INFO, "-----------------------");
    syslog(LOG_INFO, "Started Fidistat Server");

    // Daemonize
    struct pidfh *pfh = daemon_start('s');

    int kq = kqueue();

    // Set SIGTERM
    signal(SIGTERM, handleSigterm_S);
    addEvent(kq, SIGTERM, EVFILT_SIGNAL, EV_ADD, 0, 0, NULL);

    // Initialize TLS structs
    initConf();
    tls_init();
    struct tls* ctx = tls_server();
    initTLS_S(ctx);

    // Create and bind sockets
    int sock[MAXSOCK] = { 0 };
    int nsock = 0;

    struct addrinfo *servinfo, *p;
    servinfo = getAddrInfo();

    for(p = servinfo; p != NULL && nsock < MAXSOCK; p = p->ai_next) {
        if ((sock[nsock] = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            syslog(LOG_ERR, "socket error");
            continue;
        }
        int flags = fcntl(sock[nsock],F_GETFL,0);
        fcntl(sock[nsock], F_SETFL, flags | O_NONBLOCK);

        if (bind(sock[nsock], p->ai_addr, p->ai_addrlen) == -1) {
            close(sock[nsock]);
            syslog(LOG_ERR, "bind error");
            continue;
        }
        listen(sock[nsock], 5);
        nsock++;
    }

    // Add all sockets to kq
    for (int i = 0; i < nsock; i++) {
        addEvent(kq, sock[i], EVFILT_READ, EV_ADD, 0, 5, (void*)servinfo);
    }

    // Destroy Config
    destroyConf(); 
    freeaddrinfo(servinfo);

    struct kevent evList[MAXCONCUR];
    int nev;

    while(!term) {
        nev = kevent(kq, NULL, 0, evList, MAXCONCUR, NULL);
        if (nev < 0) {
            syslog(LOG_ERR, "kevent error:\n%m\n");
            break;
        }
        for (int i=0; i<nev; i++) {
            //syslog(LOG_DEBUG, "ident: %lu, filter:%i flags: %x, fflags: %u data: %ld", evList[i].ident, evList[i].filter, evList[i].flags,evList[i].fflags, evList[i].data);
            if (evList[i].filter == EVFILT_SIGNAL && evList[i].ident == SIGTERM) {
                break;
            }
            else if (evList[i].flags & EV_EOF && evList[i].udata != (void*) servinfo) {
                addEvent(kq, evList[i].ident, EVFILT_READ, EV_DELETE, 0, 0, NULL);
                free(evList[i].udata);
                close(evList[i].ident);
            }
            else if (evList[i].udata == servinfo) {
                int connfd = accept(evList[i].ident, (struct sockaddr*) NULL, NULL); 
                if (connfd == -1) {
                    syslog(LOG_ERR, "accept failed:\n%m\n");
                }
                Cinfo* cid = (Cinfo*)malloc(sizeof(cid));
                addEvent(kq, connfd, EVFILT_READ, EV_ADD, 0, 0, cid);
                cid->c_fd = connfd;
                cid->header = 1;
                cid->expect = 2;
                cid->cctx = NULL;
                if (tls_accept_socket(ctx, &(cid->cctx), connfd) == -1) {
                    syslog(LOG_DEBUG, "accept failed");
                }

                //worker(evList[i].ident, ctx);
            }
            else if (evList[i].flags & EV_ERROR) {
                syslog(LOG_DEBUG, "EV_ERROR:\n%m\n");
            }
            else {
                syslog(LOG_DEBUG, "new work");
                work(evList[i].udata, evList[i].data);
                //worker(evList[i].ident, ctx);
            }
        }
    }

    syslog(LOG_INFO, "Shutting down Server");
    for (int i = 0; i < nsock; i++) {
        close(sock[i]);
    }
    tls_close(ctx);
    tls_free(ctx);
    tls_config_free(tlsServer_conf);

    pidfile_remove(pfh);
    syslog(LOG_INFO, "Stopped Fidistat Server");
    closelog();
    exit(0);
}

void addEvent(int kq, uintptr_t ident, short filter, u_short flags, u_int fflags, intptr_t data, void *udata) {
    struct kevent evSet;

    EV_SET(&evSet, ident, filter, flags, fflags, data, udata);
    if (kevent(kq, &evSet, 1, NULL, 0, NULL) == -1) {
        syslog(LOG_ERR, "kevent set error:\n%m\n");
    }
}

void work(Cinfo* cid, size_t backlog) { 
    ssize_t ret;
    syslog(LOG_DEBUG, "Backlog: %zu", backlog);
    while (backlog > 0) {
        if (cid->header == 1 && (backlog == 1)) { 
            syslog(LOG_DEBUG, "header and only one byte");
            return;
        } else if (cid->header == 1) {
            syslog(LOG_DEBUG, "header");
            char buf[3];
            ret = tls_read(cid->cctx, &(buf), 2); 
            syslog(LOG_DEBUG, "toread: %s", buf);

            if (ret == TLS_WANT_POLLIN || ret == TLS_WANT_POLLOUT) { 
                syslog(LOG_DEBUG, "READ/WRITE AGAIN");
                continue;
                /* retry.  May use select to wait for nonblocking */ 
            } else if (ret < 0) { 
                syslog(LOG_ERR, "%s\n", tls_error(cid->cctx));
                break;
            } else { 
                cid->header = 0;
                cid->buffer = (char*)malloc((cid->expect +1) *sizeof(char));
                backlog -= ret;
                syslog(LOG_DEBUG, "rest Backlog: %zu", backlog);
            } 
        } else {
            ret = tls_read(cid->cctx, &(cid->buffer) + cid->read, cid->expect); 

            if (ret == TLS_WANT_POLLIN || ret == TLS_WANT_POLLOUT) { 
                return;
            } else if (ret < 0) { 
                syslog(LOG_ERR, "%s\n", tls_error(cid->cctx));
                break;
            } else { 
                backlog -= ret;
                cid->expect -= ret;
                cid->read += ret;
                syslog(LOG_DEBUG, "rest Backlog: %zu", backlog);
            } 
        }
        if (cid->expect == 0) {
            syslog(LOG_DEBUG, "all data read");
            syslog(LOG_DEBUG, "Output: %s", cid->buffer);
            free(cid->buffer);
            cid->header = 1;
        }
    }
}

void worker(int connfd, struct tls* ctx) {

    // Process HEADER
    //--------------
    struct tls* cctx = NULL;
    tls_accept_socket(ctx, &cctx, connfd);
    json_t *header = recvOverTLS(cctx);

    // Check client auth
    const char* clientAuth = json_string_value(json_object_get(header, "auth"));
    if (clientAuth == NULL || strcmp(clientAuth, getClientAuth()) != 0) {
        syslog(LOG_ERR, "Authentication failed");
        return;
    }

    const char* clientName = json_string_value(json_object_get(header, "from"));
    if (clientName == NULL) {
        syslog(LOG_ERR, "Header invalid");
    }
    for (size_t i = 0; i < sizeof(clientName); i++) {
        if (clientName[i] == '/' || clientName[i] == '\\') {
            syslog(LOG_ERR, "ERROR in clientName!, aborting");
            return;
        }
    }
    connType type = json_integer_value(json_object_get(header, "type"));
    int size = json_integer_value(json_object_get(header, "size"));
    syslog(LOG_DEBUG, "New Message from %s", clientName);

    // Process Payload
    //----------------

    // new Values for graphs
    if (type == NEWDATA) {
        for (int i = 0; i < size; i++) {
            json_t* payload = recvOverTLS(cctx);
            pasteJSON(payload, clientName);
        }
    } 

    // create .json or update displaysettings
    if (type == CREATE || type == UPDATE) {
        for (int i = 0; i < size; i++) {
            json_t* payload = recvOverTLS(cctx);
            const char * name = json_string_value(json_object_get(payload, "name"));
            if (name == NULL) {
                syslog(LOG_ERR, "Error in Message from Client");
                return;
            }

            json_t *root = json_object_get(payload, "payload");
            char* file = composeFileName(clientName, name, "json");
            if (type == CREATE) {
                // Create/Replace file
                dumpJSON(root, file);
            } else {
                // Update old file with new settings
                mergeJSON(root,file);
            }
        }
    }

    // Look which files are not available
    if (type == HELLO) {
        json_t *relist = json_array();
        json_t *list = recvOverTLS(cctx);

        for (int i = 0; i < size; i++) {
            const char * name = json_string_value(json_array_get(list, i));
            if (name == NULL) {
                syslog(LOG_ERR, "Error in Message from Client");
                return;
            }
            char* file = composeFileName(clientName, name, "json");
            if (access( file, F_OK ) == -1) {
                json_array_append_new(relist, json_string(name)); 
            }
        }
        // Send answer
        char * relistStr = json_dumps(relist, JSON_COMPACT);
        sendOverTLS(cctx, relistStr);
        free(relistStr);
    }
    
    // Delete all files from this client
    if (type == DELETE) {
        json_t *list = recvOverTLS(cctx);

        for (int i = 0; i < size; i++) {
            const char * name = json_string_value(json_array_get(list, i));
            delete(clientName, name);
        }
    }

    tls_close(cctx);
    tls_free(cctx);
    close(connfd);

}

void initTLS_S(struct tls* ctx) {
    tlsServer_conf = tls_config_new();
    tls_config_set_cert_file(tlsServer_conf, getServerCertFile_v());
    tls_config_set_key_file(tlsServer_conf, getServerCertFile_v());

    tls_configure(ctx, tlsServer_conf);
}

struct addrinfo* getAddrInfo() {
    struct addrinfo hints, *servinfo;

    memset(&hints, 0, sizeof hints);

    hints.ai_family =  PF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP address

    getaddrinfo(NULL, getClientServerPort(), &hints, &servinfo);
    
    return servinfo;

}
