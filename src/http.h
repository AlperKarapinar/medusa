
#if !defined(MEDUSA_HTTP_H)
#define MEDUSA_HTTP_H

enum {
        MEDUSA_HTTP_PROTOCOL_ANY        = 0,
        MEDUSA_HTTP_PROTOCOL_IPV4       = 1,
        MEDUSA_HTTP_PROTOCOL_IPV6       = 2
#define MEDUSA_HTTP_PROTOCOL_ANY        MEDUSA_HTTP_PROTOCOL_ANY
#define MEDUSA_HTTP_PROTOCOL_IPV4       MEDUSA_HTTP_PROTOCOL_IPV4
#define MEDUSA_HTTP_PROTOCOL_IPV6       MEDUSA_HTTP_PROTOCOL_IPV6
};

enum {
        MEDUSA_HTTP_OPEN_MODE_READ      = 0x00000001,
        MEDUSA_HTTP_OPEN_MODE_WRITE     = 0x00000002
#define MEDUSA_HTTP_OPEN_MODE_READ      MEDUSA_HTTP_OPEN_MODE_READ
#define MEDUSA_HTTP_OPEN_MODE_WRITE     MEDUSA_HTTP_OPEN_MODE_WRITE
};

enum {
        MEDUSA_HTTP_SEEK_WHENCE_SET     = 0,
        MEDUSA_HTTP_SEEK_WHENCE_CUR     = 1,
        MEDUSA_HTTP_SEEK_WHENCE_END     = 2
#define MEDUSA_HTTP_SEEK_WHENCE_SET     MEDUSA_HTTP_SEEK_WHENCE_SET
#define MEDUSA_HTTP_SEEK_WHENCE_CUR     MEDUSA_HTTP_SEEK_WHENCE_CUR
#define MEDUSA_HTTP_SEEK_WHENCE_END     MEDUSA_HTTP_SEEK_WHENCE_END
};

struct medusa_http_stat {
        long long size;
        long long mtime;
        int seekable;
        int chunked;
};

#endif
