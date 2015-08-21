#include <tls.h>
#include <syslog.h>
#include "tls.h"

void sendOverTLS(struct tls* ctx, void *buf, size_t size) {
    size_t toSend = size;
    size_t sent;
    while (toSend > 0) {
        int ret = tls_read(ctx, buf, size, &sent); 
 
        if (ret == TLS_READ_AGAIN || ret == TLS_WRITE_AGAIN) { 
            // retry.  May use select to wait for nonblocking
        } else if (ret < 0) { 
            syslog(LOG_ERR, "%s\n", tls_error(ctx));
            break;
        } else { 
            buf += sent;
            toSend -= sent;
        }
    }
}

void recvOverTLS(struct tls*ctx, size_t size, void *buf) {
}

void waitforACK() {

}
