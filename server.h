#ifndef SERVER
#define SERVER
#include <tls.h>
#include "main.h"

void server();
void worker(int connfd, struct tls* ctx);

int initTLS_S(struct tls* ctx);
struct tls_config* tlsServer_conf;
struct addrinfo* getAddrInfo();

#endif
