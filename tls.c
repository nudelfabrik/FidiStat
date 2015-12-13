#include <tls.h>
#include <arpa/inet.h>
#include <syslog.h>
#include <string.h>
#include <stdlib.h>
#include <jansson.h>
#include "tls.h"

void sendOverTLS(struct tls* ctx, const char *buf) {
    size_t sent;

    // send Length of buf
    syslog(LOG_DEBUG, "Send: Size: %i", length);
    uint16_t length = htons(strlen(buf));
    syslog(LOG_DEBUG, "Send: Size after: %i", length);

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

    // send actual buf
    size_t toSend = strlen(buf);
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

json_t* recvOverTLS(struct tls*ctx) {
    json_error_t error;
    size_t size;
    uint16_t length;
    size_t len = sizeof(length);

    // read length
    while (len > 0) {
        int ret = tls_read(ctx, &length, len, &size); 
 
        if (ret == TLS_READ_AGAIN || ret == TLS_WRITE_AGAIN) { 
            /* retry.  May use select to wait for nonblocking */ 
        } else if (ret < 0) { 
            syslog(LOG_ERR, "%s\n", tls_error(ctx));
            break;
        } else { 
            len -= size; 
        } 
    }
    syslog(LOG_DEBUG, "Size: %i", length);

    // Change length to Hardware Byte Order
    length = ntohs(length);
    syslog(LOG_DEBUG, "Size after: %i", length);
    // create buffer
    char* buffer = (char*)malloc((length +1) *sizeof(char));
    char* buf = buffer;

    while (length > 0) {
        int ret = tls_read(ctx, buf, length, &size); 
 
        if (ret == TLS_READ_AGAIN || ret == TLS_WRITE_AGAIN) { 
            /* retry.  May use select to wait for nonblocking */ 
        } else if (ret < 0) { 
            syslog(LOG_ERR, "%s\n", tls_error(ctx));
            break;
        } else { 
            buf += size; 
            length -= size; 
        } 
    }
    syslog(LOG_DEBUG, "buffer: %s", buf);

    // Process buffer to json
    json_t *json = json_loads(buffer, JSON_DISABLE_EOF_CHECK, &error);
    if (verbose_flag) {
        syslog(LOG_DEBUG, "%s\n", buffer);
    }
    return json;
}
