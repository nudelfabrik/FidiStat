#include <tls.h>
#include <arpa/inet.h>
#include <syslog.h>
#include <string.h>
#include <stdlib.h>
#include <jansson.h>
#include "tls.h"

void sendOverTLS(struct tls* ctx, const char *buf) {
    ssize_t ret;

    // send Length of buf
    uint16_t length = htons(strlen(buf));

    size_t len = sizeof(length);
    while (len > 0) {
        ret = tls_write(ctx, &length, len); 
 
        if (ret == TLS_WANT_POLLIN || ret == TLS_WANT_POLLOUT) { 
            syslog(LOG_DEBUG, "READ/WRITE AGAIN\n");
        } else if (ret < 0) { 
            syslog(LOG_ERR, "%s\n", tls_error(ctx));
            break;
        } else { 
            len -= ret;
        }
    }

    // send actual buf
    size_t toSend = strlen(buf);
    while (toSend > 0) {
        ret = tls_write(ctx, buf, toSend); 
 
        if (ret == TLS_WANT_POLLIN || ret == TLS_WANT_POLLOUT) { 
            syslog(LOG_DEBUG, "READ/WRITE AGAIN\n");
        } else if (ret < 0) { 
            syslog(LOG_ERR, "%s\n", tls_error(ctx));
            break;
        } else { 
            buf += ret;
            toSend -= ret;
        }
    }
}

json_t* recvOverTLS(struct tls*ctx) {
    json_error_t error;
    ssize_t ret;
    uint16_t length;
    size_t len = sizeof(length);

    // read length
    while (len > 0) {
        ret = tls_read(ctx, &length, len); 
 
        if (ret == TLS_WANT_POLLIN || ret == TLS_WANT_POLLOUT) { 
            /* retry.  May use select to wait for nonblocking */ 
        } else if (ret < 0) { 
            syslog(LOG_ERR, "%s\n", tls_error(ctx));
            break;
        } else { 
            len -= ret; 
        } 
    }

    // Change length to Hardware Byte Order
    length = ntohs(length);

    // create buffer
    char* buffer = (char*)malloc((length +1) *sizeof(char));
    char* buf = buffer;

    while (length > 0) {
        ret = tls_read(ctx, buf, length); 
 
        if (ret == TLS_WANT_POLLIN || ret == TLS_WANT_POLLOUT) { 
            /* retry.  May use select to wait for nonblocking */ 
        } else if (ret < 0) { 
            syslog(LOG_ERR, "%s\n", tls_error(ctx));
            break;
        } else { 
            buf += ret; 
            length -= ret; 
        } 
    }

    // Process buffer to json
    json_t *json = json_loads(buffer, JSON_DISABLE_EOF_CHECK, &error);
    if (verbose_flag) {
        syslog(LOG_DEBUG, "%s\n", buffer);
    }
    return json;
}
