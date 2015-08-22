#include <sys/types.h> 
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <tls.h>
#include "server.h"
#include "config.h"
#include "client.h"
#include "tls.h"

void server() {

char *cfgLocation = "/usr/local/etc/fidistat/config.cfg";
    // load Config File and Settings
    initConf(cfgLocation);
    tls_init();
    struct tls* ctx = tls_server();
    int sock = initTLS_S(ctx);

    int connfd, pid;
    listen(sock, 10);
    while(1) {
        connfd = accept(sock, (struct sockaddr*) NULL, NULL); 

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
}
void worker(int connfd, struct tls* ctx) {
    struct tls* cctx = NULL;
    tls_accept_socket(ctx, &cctx, connfd);
    char* header = recvOverTLS(cctx);
    syslog(LOG_DEBUG,"Received: %s\n", header);

}

int initTLS_S(struct tls* ctx) {
    tlsServer_conf = tls_config_new();
    printf("Cert: %s\n", getServerCertFile());
    tls_config_set_cert_file(tlsServer_conf, getServerCertFile());
    tls_config_set_key_file(tlsServer_conf, getServerCertFile());

    tls_configure(ctx, tlsServer_conf);

    int sock;
    struct sockaddr_in serv_addr;
    if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        printf("socket conn failed\n");
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    //serv_addr.sin6_port = htons(getServerPort());
    serv_addr.sin_port = htons(4242);
    if (bind(sock, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
        printf("ERROR on binding\n");
    }
    printf("start socket\n");
    return sock;
}
