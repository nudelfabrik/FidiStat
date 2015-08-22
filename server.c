#include <sys/types.h> 
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <tls.h>
#include "server.h"
#include "config.h"
#include "client.h"

void server() {

char *cfgLocation = "/usr/local/etc/fidistat/config.cfg";
    // load Config File and Settings
    initConf(cfgLocation);
    struct tls* ctx = tls_server();
    int sock = initTLS_S(ctx);

    int connfd;
    if (verbose_flag || now_flag) {
        while(1)
        {
            connfd = accept(sock, (struct sockaddr*) NULL, NULL); 
            struct tls* cctx = NULL;
            tls_accept_socket(ctx, &cctx, connfd);
            size_t getSize, size;
            int ret = tls_read(ctx, &getSize, 1, &size); 

            char buffer[1024];
            char* buf = buffer;

            while (getSize > 0) {
                int ret = tls_read(cctx, buf, getSize, &size); 
         
                if (ret == TLS_READ_AGAIN || ret == TLS_WRITE_AGAIN) { 
                    /* retry.  May use select to wait for nonblocking */ 
                } else if (ret < 0) { 
                    printf("%s\n", tls_error(ctx));
                    break;
                } else { 
                    buf += size; 
                    getSize -= size; 
                } 
            }
            printf("%s", buffer);
        }
    }
}

int initTLS_S(struct tls* ctx) {
    tls_init();
    tlsServer_conf = tls_config_new();
    tls_config_set_cert_file(tlsServer_conf, getServerCertFile());
    tls_config_set_key_file(tlsServer_conf, getServerCertFile());

    ctx = tls_server();
    tls_configure(ctx, tlsServer_conf);

    int sock;
    struct sockaddr_in6 serv_addr;
    if ((sock = socket(PF_INET6, SOCK_STREAM, 0)) < 0) {
        printf("socket conn failed");
    }
    serv_addr.sin6_family = AF_INET6;
    serv_addr.sin6_addr = in6addr_any;
    serv_addr.sin6_port = htons(getServerPort());
    if (bind(sock, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
        printf("ERROR on binding");
    }
    listen(sock, 10);
    return sock;
}
