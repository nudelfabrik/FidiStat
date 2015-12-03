#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/event.h>
#include <netinet/in.h>
#include <arpa/inet.h>
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
    term = 1;
}

void handleChild(int sig) {
    wait(NULL);
}

void server() {
    // load Config File and Settings
    fprintf(stdout, "Starting fidistat Server...\n");
    openlog("fidistat-server", LOG_PID, LOG_DAEMON);
    syslog(LOG_INFO, "Started Fidistat Server");

    struct pidfh *pfh = daemon_start('s');

    // Handle Signals
    signal(SIGTERM, handleSigterm_S);
    signal(SIGCHLD, handleChild);

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

        if (bind(sock[nsock], p->ai_addr, p->ai_addrlen) == -1) {
            close(sock[nsock]);
            syslog(LOG_ERR, "bind error");
            continue;
        }
        listen(sock[nsock], 5);
        nsock++;
    }
    freeaddrinfo(servinfo);

    // Build kqueue
    int kq;
    struct kevent evSet;

    kq = kqueue();
    for (int i = 0; i < nsock; i++) {
        EV_SET(&evSet, s[i], EVFILT_READ, EV_ADD, 0, 0, (void *)ai0);
        kevent(kq, &evSet, 1, (void *)0, 0, (struct timespec*)0);
    }
                                                             

    int connfd, pid;

    // Destroy Config
    destroyConf(); 

    while(!term) {
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
    }
    syslog(LOG_INFO, "Shutting down Server");
    close(sock);
    tls_close(ctx);
    tls_free(ctx);
    tls_config_free(tlsServer_conf);

    pidfile_remove(pfh);
    syslog(LOG_INFO, "Stopped Fidistat Server");
    closelog();
    exit(0);
}

void worker(int connfd, struct tls* ctx) {

    // Process HEADER
    //--------------
    struct tls* cctx = NULL;
    tls_accept_socket(ctx, &cctx, connfd);
    json_t *header = recvOverTLS(cctx);
    const char* clientName = json_string_value(json_object_get(header, "from"));
    for (int i = 0; i < sizeof(clientName); i++) {
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
