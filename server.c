#include <sys/types.h> 
#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <tls.h>
#include <jansson.h>
#include "server.h"
#include "json.h"
#include "config.h"
#include "client.h"
#include "tls.h"
#include "main.h"

void server() {
char *cfgLocation = "/usr/local/etc/fidistat/config.cfg";
    // load Config File and Settings
    initConf(cfgLocation);
    tls_init();
    struct tls* ctx = tls_server();
    int sock = initTLS_S(ctx);

    int connfd, pid;
    listen(sock, 10);

    // Destroy Config
    destroyConf(); 

    //signal(SIGTERM, handleSigterm);
    while(!term) {
        connfd = accept(sock, (struct sockaddr*) NULL, NULL); 
        syslog(LOG_DEBUG, "socket: %d", connfd);

        if (term) {
            break;
        }
        pid = fork();
        if (pid < 0) {
            syslog(LOG_ERR, "forking new Worker failed");
        } else if (pid == 0) {
            close(sock);
            worker(connfd, ctx);
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
}

void worker(int connfd, struct tls* ctx) {
    struct tls* cctx = NULL;
    tls_accept_socket(ctx, &cctx, connfd);
    char* headerStr = recvOverTLS(cctx);
    syslog(LOG_DEBUG, "%s\n", headerStr);
    json_error_t error;
    json_t *header = json_loads(headerStr, 0, &error);
    const char* clientName = json_string_value(json_object_get(header, "from"));
    int type = json_integer_value(json_object_get(header, "type"));
    if (type == UPDATE) {
        int size = json_integer_value(json_object_get(header, "size"));

        for (int i = 0; i < size; i++) {
            char* payloadStr = recvOverTLS(cctx);
            syslog(LOG_DEBUG, "%s\n", payloadStr);
            json_t *payload = json_loads(payloadStr, 0, &error);
            pasteJSON(payload, clientName);
        }
    } 
    if (type == REPLACE) {
    }
    tls_close(cctx);
    tls_free(cctx);
}

int initTLS_S(struct tls* ctx) {
    tlsServer_conf = tls_config_new();
    syslog(LOG_DEBUG,"Cert: %s\n", getServerCertFile());
    tls_config_set_cert_file(tlsServer_conf, getServerCertFile());
    tls_config_set_key_file(tlsServer_conf, getServerCertFile());

    tls_configure(ctx, tlsServer_conf);

    int sock;

    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof hints);
    if(ipv6) {
        hints.ai_family =  AF_INET6;
    } else {
        hints.ai_family =  AF_INET;
    }

    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP address

    getaddrinfo(NULL, getServerPort(), &hints, &servinfo);

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
