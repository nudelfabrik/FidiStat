#include <tls.h>
#include <syslog.h>
#include <string.h>
#include "tls.h"

void sendOverTLS(struct tls* ctx, const char *buf) {
    size_t sent;

    // Length of String
    size_t length = strlen(buf);
    syslog(LOG_DEBUG, "Sending length\n");
    syslog(LOG_DEBUG, "Sending: %zu\n", length);

    size_t len = sizeof(length);
    while (len > 0) {
        int ret = tls_write(ctx, &length, len, &sent); 
        syslog(LOG_DEBUG, "Sent over TLS\n");
 
        if (ret == TLS_READ_AGAIN || ret == TLS_WRITE_AGAIN) { 
            syslog(LOG_ERR, "READ/WRITE AGAIN\n");
        } else if (ret < 0) { 
            syslog(LOG_ERR, "%s\n", tls_error(ctx));
            break;
        } else { 
            len -= sent;
        }
    }
    syslog(LOG_DEBUG, "Sent");

    syslog(LOG_DEBUG, "Sending over TLS\n");
    syslog(LOG_DEBUG, "Sending: %s\n", buf);
    size_t toSend = length;
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
