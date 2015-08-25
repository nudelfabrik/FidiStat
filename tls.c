#include <tls.h>
#include <syslog.h>
#include <string.h>
#include <stdlib.h>
#include <jansson.h>
#include "tls.h"

void sendOverTLS(struct tls* ctx, const char *buf) {
    size_t sent;

    // send Length of buf
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

    // send actual buf
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

json_t* recvOverTLS(struct tls*ctx) {
    json_error_t error;
    size_t getSize, size;
    size_t len = sizeof(getSize);

    // read length
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

    // create buffer
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

    // Process buffer to json
    json_t *json = json_loads(buffer, JSON_DISABLE_EOF_CHECK, &error);
    if (verbose_flag) {
        syslog(LOG_DEBUG, "%s\n", buffer);
    }
    return json;
}
