#include <sys/types.h> 
#include <sys/socket.h>
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

// signal Handler
int sckt;
void handleSigterm_S(int sig) {
    if (sig == SIGTERM) {
        term = 1;
        shutdown(sckt,SHUT_RDWR);
    }
}

void handleChild(int sig) {
    if (sig == SIGCHLD) {
        wait(NULL);
    }
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

    // Open Socket
    initConf();
    tls_init();
    struct tls* ctx = tls_server();
    int sock = initTLS_S(ctx);
    sckt = sock;

    int connfd, pid;
    listen(sock, 10);

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
                dumpJSON(root, file);
            } else {
                mergeJSON(root,file);
            }
        }
    }

    // Look, which files are not available
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

int initTLS_S(struct tls* ctx) {
    tlsServer_conf = tls_config_new();
    tls_config_set_cert_file(tlsServer_conf, getServerCertFile_v());
    tls_config_set_key_file(tlsServer_conf, getServerCertFile_v());

    tls_configure(ctx, tlsServer_conf);

    int sock;

    struct addrinfo *servinfo, *p;
    servinfo = getAddrInfo();

    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sock = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            syslog(LOG_ERR, "socket error");
            continue;
        }

        if (bind(sock, p->ai_addr, p->ai_addrlen) == -1) {
            close(sock);
            syslog(LOG_ERR, "bind error");
            continue;
        }
        break; // if we get here, we must have connected successfully
    }

    freeaddrinfo(servinfo);
    syslog(LOG_DEBUG, "start socket\n");
    return sock;
}

struct addrinfo* getAddrInfo() {
    struct addrinfo hints, *servinfo;

    memset(&hints, 0, sizeof hints);
    if(getServerIPv6_v()) {
        hints.ai_family =  AF_INET6;
    } else {
        hints.ai_family =  AF_INET;
    }

    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP address

    getaddrinfo(NULL, getClientServerPort(), &hints, &servinfo);
    
    return servinfo;

}
