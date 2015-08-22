#ifndef TLSH
#define TLSH
#include <tls.h>

void sendOverTLS(struct tls* ctx, const char *buf);
char* recvOverTLS(struct tls*ctx);
void waitforACK();

#endif
