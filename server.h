#ifndef SERVER
#define SERVER
#include <tls.h>

void server();
int initTLS_S(struct tls* ctx);
struct tls_config* tlsServer_conf;
const char *port_S;

#endif
