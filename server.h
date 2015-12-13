#ifndef SERVER
#define SERVER
#include <tls.h>
#include "main.h"

struct connInfo {
    int c_fd;
    short header;
    uint16_t expect;
    char* buffer;
    size_t read;
    struct tls* cctx;
};
typedef struct connInfo Cinfo;
void server();
void work(Cinfo* cid, size_t backlog);
void worker(int connfd, struct tls* ctx);
void addEvent(int kq, uintptr_t ident, short filter, u_short flags, u_int fflags, intptr_t data, void *udata);

void initTLS_S(struct tls* ctx);
struct tls_config* tlsServer_conf;
struct addrinfo* getAddrInfo();

#endif

