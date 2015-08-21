#ifndef TLSH
#define TLSH
#include <tls.h>

void sendOverTLS(struct tls* ctx, void *buf, size_t size);
void recvOverTLS(struct tls*ctx, size_t size, void *buf);
void waitforACK();

#endif
