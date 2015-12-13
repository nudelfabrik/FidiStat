#ifndef SERVER
#define SERVER
#include <tls.h>
#include "main.h"

struct connInfo {
    int c_fd;
    char* buffer;
    struct tls* cctx;
};
typedef struct connInfo Cinfo;
void server();
void worker(int connfd, struct tls* ctx);
void addEvent(int kq, uintptr_t ident, short filter, u_short flags, u_int fflags, intptr_t data, void *udata);

void initTLS_S(struct tls* ctx);
struct tls_config* tlsServer_conf;
struct addrinfo* getAddrInfo();

#endif

