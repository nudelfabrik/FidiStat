#include <tls.h>
#include <syslog.h>
#include <string.h>
#include <stdlib.h>
#include "tls.h"

void sendOverTLS(struct tls* ctx, const char *buf) {
    size_t sent;

    // Length of String
    size_t length = strlen(buf);

    size_t len = sizeof(length);
    while (len > 0) {
        int ret = tls_write(ctx, &length, len, &sent); 
 
        if (ret == TLS_READ_AGAIN || ret == TLS_WRITE_AGAIN) { 
            syslog(LOG_DEBUG, "READ/WRITE AGAIN\n");
        } else if (ret < 0) { 
            syslog(LOG_ERR, "%s\n", tls_error(ctx));
            break;
        } else { 
            len -= sent;
        }
    }

    size_t toSend = length;
    while (toSend > 0) {
        int ret = tls_write(ctx, buf, toSend, &sent); 
 
        if (ret == TLS_READ_AGAIN || ret == TLS_WRITE_AGAIN) { 
            syslog(LOG_DEBUG, "READ/WRITE AGAIN\n");
        } else if (ret < 0) { 
            syslog(LOG_ERR, "%s\n", tls_error(ctx));
            break;
        } else { 
            buf += sent;
            toSend -= sent;
        }
    }
}

char* recvOverTLS(struct tls*ctx) {
    size_t getSize, size;
    size_t len = sizeof(getSize);

    while (len > 0) {
        int ret = tls_read(ctx, &getSize, len, &size); 
 
        if (ret == TLS_READ_AGAIN || ret == TLS_WRITE_AGAIN) { 
            /* retry.  May use select to wait for nonblocking */ 
        } else if (ret < 0) { 
            syslog(LOG_ERR, "%s\n", tls_error(ctx));
            break;
        } else { 
            len -= size; 
        } 
    }
    char* buffer = (char*)malloc((getSize +1) *sizeof(char));
    char* buf = buffer;

    while (getSize > 0) {
        int ret = tls_read(ctx, buf, getSize, &size); 
 
        if (ret == TLS_READ_AGAIN || ret == TLS_WRITE_AGAIN) { 
            /* retry.  May use select to wait for nonblocking */ 
        } else if (ret < 0) { 
            syslog(LOG_ERR, "%s\n", tls_error(ctx));
            break;
        } else { 
            buf += size; 
            getSize -= size; 
        } 
    }
    return buffer;
}

void waitforACK() {

}
