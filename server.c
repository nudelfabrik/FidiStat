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

    struct pidfh *pfh = daemon_start('s');
    signal(SIGTERM, handleSigterm_S);

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
        //int flags = fcntl(sock[nsock],F_GETFL,0);
        //fcntl(sock[nsock], F_SETFL, flags | O_NONBLOCK);

        if (bind(sock[nsock], p->ai_addr, p->ai_addrlen) == -1) {
            close(sock[nsock]);
            syslog(LOG_ERR, "bind error");
            continue;
        }
        listen(sock[nsock], 5);
        syslog(LOG_DEBUG, "Opened Socket: %i", sock[nsock]);
        nsock++;
    }
    // Build kqueue
    int kq = kqueue();

    // Add all sockets to kq
    syslog(LOG_DEBUG, "Setting %i kqueue triggers", nsock);
    syslog(LOG_DEBUG, "void servinfo: %p", (void*)servinfo);
    for (int i = 0; i < nsock; i++) {
        addEvent(kq, sock[i], EVFILT_READ, EV_ADD, 0, 5, (void*)servinfo);
    }
    // fire kevent when SIGTERM
    addEvent(kq, SIGTERM, EVFILT_SIGNAL, EV_ADD, 0, 0, NULL);

    // Destroy Config
    destroyConf(); 
    //freeaddrinfo(servinfo);

    struct kevent evList[32];
    int nev;

    while(!term) {
        nev = kevent(kq, NULL, 0, evList, 32, NULL);
        syslog(LOG_INFO, "New events: %i", nev);
        if (nev < 0) {
            syslog(LOG_ERR, "kevent error:\n%m\n");
            break;
        }
        for (int i=0; i<nev; i++) {
            if (evList[i].filter == EVFILT_SIGNAL && evList[i].ident == SIGTERM) {
                syslog(LOG_INFO, "SIGTERM received");
                break;
            }
            else if (evList[i].flags & EV_EOF && evList[i].udata != (void*) servinfo) {
                syslog(LOG_DEBUG, "Received EOF");
                addEvent(kq, evList[i].ident, EVFILT_READ, EV_DELETE, 0, 0, NULL);
                syslog(LOG_DEBUG, "Connection closed with EOF");
                close(evList[i].ident);
            }
            else if (evList[i].udata == servinfo) {
                syslog(LOG_DEBUG, "Accepting new connection on socket %lu", evList[i].ident);
                int connfd = accept(evList[i].ident, (struct sockaddr*) NULL, NULL); 
                syslog(LOG_DEBUG, "Connection accepted, fd: %i", connfd);
                if (connfd == -1) {
                    syslog(LOG_ERR, "accept failed:\n%m\n");
                }
                addEvent(kq, connfd, EVFILT_READ, EV_ADD, 0, 0, NULL);
    //tls_accept_socket(ctx, &cctx, connfd);
    //            char buf[evList[i].data -29];
    //            size_t size;
    //            int ret = tls_read(cctx, buf, evList[i].data -30, &size); 
    //            syslog(LOG_DEBUG, "Output: %s", buf);
    //            char buf[10] = "Test\n";
    //            send(connfd, buf, sizeof(buf), 0);
            }
            else if (evList[i].flags & EVFILT_READ) {
                syslog(LOG_DEBUG, "New data on connection");
                worker(evList[i].ident, ctx);
            }
            else if (evList[i].flags & EV_ERROR) {
                syslog(LOG_DEBUG, "EV_ERROR:\n%m\n");
            }
            else {
                syslog(LOG_DEBUG, "other case");
                syslog(LOG_DEBUG, "ident: %lu, filter:%i flags: %x, fflags: %u bytes: %ld", evList[i].ident, evList[i].filter, evList[i].flags,evList[i].fflags, evList[i].data);
                worker(evList[i].ident, ctx);
                //char buf[evList[i].data];
                //recv(evList[i].ident, buf, evList[i].data -1, 0);
                //char buf[1024] = {0};
                //size_t size;
                //int ret = tls_read(cctx, buf, evList[i].data -30, &size); 
                //syslog(LOG_DEBUG, "Output: %s", buf);
                //term = 1;
                
            }
        }

        /*
        connfd = accept(sock, (struct sockaddr*) NULL, NULL); 

        if (term) {
            break;
        }
        pid = fork();
        if (pid < 0) {
            syslog(LOG_ERR, "forking new Worker failed");
        } else if (pid == 0) {
            close(sock);
            syslog(LOG_INFO, "New incoming connection");
            worker(connfd, ctx);
            syslog(LOG_INFO, "Closing connection");
            exit(0);
        } else {
            close(connfd);
        }
        */
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

void worker(int connfd, struct tls* ctx) {

    // Process HEADER
    //--------------
    struct tls* cctx = NULL;
    tls_accept_socket(ctx, &cctx, connfd);
    syslog(LOG_DEBUG, "processing sent data");
    json_t *header = recvOverTLS(cctx);
    syslog(LOG_DEBUG, "received processed");
    const char* clientName = json_string_value(json_object_get(header, "from"));
    for (size_t i = 0; i < sizeof(clientName); i++) {
        if (clientName[i] == '/' || clientName[i] == '\\') {
            syslog(LOG_ERR, "ERROR in clientName!, aborting");
            return;
        }
    }
    const char* clientAuth = json_string_value(json_object_get(header, "auth"));
    if (strcmp(clientAuth, getClientAuth()) != 0) {
        syslog(LOG_ERR, "Authentication failed");
        return;
    }
    connType type = json_integer_value(json_object_get(header, "type"));
    int size = json_integer_value(json_object_get(header, "size"));
    syslog(LOG_DEBUG, "Payload received");

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
