#include <tls.h>
#include <syslog.h>
#include "tls.h"

void sendOverTLS(struct tls* ctx, void *buf, size_t size) {
    size_t toSend = size;
    size_t sent;
    syslog(LOG_DEBUG, "Sending over TLS\n");
    syslog(LOG_DEBUG, "sent %zu", size);
    while (toSend > 0) {
        int ret = tls_write(ctx, buf, toSend, &sent); 
        syslog(LOG_DEBUG, "Sent over TLS\n");
 
        if (ret == TLS_READ_AGAIN || ret == TLS_WRITE_AGAIN) { 
            syslog(LOG_ERR, "READ/WRITE AGAIN\n");
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
