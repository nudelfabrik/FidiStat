#ifndef TLSH
#define TLSH
#include <tls.h>

void sendOverTLS(struct tls* ctx, const char *buf);
void recvOverTLS(struct tls*ctx, size_t size, void *buf);
void waitforACK();

#endif
