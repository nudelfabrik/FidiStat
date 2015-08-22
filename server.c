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
    tls_init();
    struct tls* ctx = tls_server();
    int sock = initTLS_S(ctx);

    int connfd;
    if (verbose_flag || now_flag) {
        listen(sock, 10);
        while(1)
        {
            connfd = accept(sock, (struct sockaddr*) NULL, NULL); 
            printf("test\n");
            struct tls* cctx = NULL;
            tls_accept_socket(ctx, &cctx, connfd);
            size_t getSize, size;
            size_t len = sizeof(getSize);

            while (len > 0) {
                int ret = tls_read(cctx, &getSize, len, &size); 
         
                if (ret == TLS_READ_AGAIN || ret == TLS_WRITE_AGAIN) { 
                    /* retry.  May use select to wait for nonblocking */ 
                } else if (ret < 0) { 
                    printf("%s\n", tls_error(cctx));
                    break;
                } else { 
                    len -= size; 
                } 
            }
            printf("%zu\n", getSize);
            char buffer[1024];
            char* buf = buffer;

            while (getSize > 0) {
                int ret = tls_read(cctx, buf, getSize, &size); 
         
                if (ret == TLS_READ_AGAIN || ret == TLS_WRITE_AGAIN) { 
                    /* retry.  May use select to wait for nonblocking */ 
                } else if (ret < 0) { 
                    printf("%s\n", tls_error(cctx));
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
