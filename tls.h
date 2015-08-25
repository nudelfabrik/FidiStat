#ifndef TLSH
#define TLSH
#include "main.h"

void sendOverTLS(struct tls* ctx, const char *buf);
json_t* recvOverTLS(struct tls*ctx);

#endif
