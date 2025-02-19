
#if !defined(MEDUSA_HTTPREQUEST_H)
#define MEDUSA_HTTPREQUEST_H

struct medusa_monitor;
struct medusa_httprequest;
struct medusa_httprequest_reply;
struct medusa_httprequest_reply_header;
struct medusa_httprequest_reply_headers;
struct medusa_httprequest_reply_body;

enum {
        MEDUSA_HTTPREQUEST_PROTOCOL_ANY                = 0,
        MEDUSA_HTTPREQUEST_PROTOCOL_IPV4               = 1,
        MEDUSA_HTTPREQUEST_PROTOCOL_IPV6               = 2
#define MEDUSA_HTTPREQUEST_PROTOCOL_ANY                MEDUSA_HTTPREQUEST_PROTOCOL_ANY
#define MEDUSA_HTTPREQUEST_PROTOCOL_IPV4               MEDUSA_HTTPREQUEST_PROTOCOL_IPV4
#define MEDUSA_HTTPREQUEST_PROTOCOL_IPV6               MEDUSA_HTTPREQUEST_PROTOCOL_IPV6
};

enum {
        MEDUSA_HTTPREQUEST_EVENT_RESOLVING              = 0x00000001,
        MEDUSA_HTTPREQUEST_EVENT_RESOLVE_TIMEOUT        = 0x00000002,
        MEDUSA_HTTPREQUEST_EVENT_RESOLVED               = 0x00000004,
        MEDUSA_HTTPREQUEST_EVENT_CONNECTING             = 0x00000008,
        MEDUSA_HTTPREQUEST_EVENT_CONNECT_TIMEOUT        = 0x00000010,
        MEDUSA_HTTPREQUEST_EVENT_CONNECTED              = 0x00000020,
        MEDUSA_HTTPREQUEST_EVENT_REQUESTING             = 0x00000040,
        MEDUSA_HTTPREQUEST_EVENT_REQUEST_TIMEOUT        = 0x00000080,
        MEDUSA_HTTPREQUEST_EVENT_REQUESTED              = 0x00000100,
        MEDUSA_HTTPREQUEST_EVENT_RECEIVING              = 0x00000200,
        MEDUSA_HTTPREQUEST_EVENT_RECEIVE_TIMEOUT        = 0x00000400,
        MEDUSA_HTTPREQUEST_EVENT_RECEIVED               = 0x00000800,
        MEDUSA_HTTPREQUEST_EVENT_DISCONNECTED           = 0x00001000,
        MEDUSA_HTTPREQUEST_EVENT_DESTROY                = 0x00002000
#define MEDUSA_HTTPREQUEST_EVENT_RESOLVING              MEDUSA_HTTPREQUEST_EVENT_RESOLVING
#define MEDUSA_HTTPREQUEST_EVENT_RESOLVE_TIMEOUT        MEDUSA_HTTPREQUEST_EVENT_RESOLVE_TIMEOUT
#define MEDUSA_HTTPREQUEST_EVENT_RESOLVED               MEDUSA_HTTPREQUEST_EVENT_RESOLVED
#define MEDUSA_HTTPREQUEST_EVENT_CONNECTING             MEDUSA_HTTPREQUEST_EVENT_CONNECTING
#define MEDUSA_HTTPREQUEST_EVENT_CONNECT_TIMEOUT        MEDUSA_HTTPREQUEST_EVENT_CONNECT_TIMEOUT
#define MEDUSA_HTTPREQUEST_EVENT_CONNECTED              MEDUSA_HTTPREQUEST_EVENT_CONNECTED
#define MEDUSA_HTTPREQUEST_EVENT_REQUESTING             MEDUSA_HTTPREQUEST_EVENT_REQUESTING
#define MEDUSA_HTTPREQUEST_EVENT_REQUEST_TIMEOUT        MEDUSA_HTTPREQUEST_EVENT_REQUEST_TIMEOUT
#define MEDUSA_HTTPREQUEST_EVENT_REQUESTED              MEDUSA_HTTPREQUEST_EVENT_REQUESTED
#define MEDUSA_HTTPREQUEST_EVENT_RECEIVING              MEDUSA_HTTPREQUEST_EVENT_RECEIVING
#define MEDUSA_HTTPREQUEST_EVENT_RECEIVE_TIMEOUT        MEDUSA_HTTPREQUEST_EVENT_RECEIVE_TIMEOUT
#define MEDUSA_HTTPREQUEST_EVENT_RECEIVED               MEDUSA_HTTPREQUEST_EVENT_RECEIVED
#define MEDUSA_HTTPREQUEST_EVENT_DISCONNECTED           MEDUSA_HTTPREQUEST_EVENT_DISCONNECTED
#define MEDUSA_HTTPREQUEST_EVENT_DESTROY                MEDUSA_HTTPREQUEST_EVENT_DESTROY
};

enum {
        MEDUSA_HTTPREQUEST_STATE_UNKNWON                = 0,
        MEDUSA_HTTPREQUEST_STATE_DISCONNECTED           = 1,
        MEDUSA_HTTPREQUEST_STATE_RESOLVING              = 2,
        MEDUSA_HTTPREQUEST_STATE_RESOLVED               = 3,
        MEDUSA_HTTPREQUEST_STATE_CONNECTING             = 4,
        MEDUSA_HTTPREQUEST_STATE_CONNECTED              = 5,
        MEDUSA_HTTPREQUEST_STATE_REQUESTING             = 6,
        MEDUSA_HTTPREQUEST_STATE_REQUESTED              = 7,
        MEDUSA_HTTPREQUEST_STATE_RECEIVING              = 8,
        MEDUSA_HTTPREQUEST_STATE_RECEIVED               = 0,
#define MEDUSA_HTTPREQUEST_STATE_UNKNWON                MEDUSA_HTTPREQUEST_STATE_UNKNWON
#define MEDUSA_HTTPREQUEST_STATE_DISCONNECTED           MEDUSA_HTTPREQUEST_STATE_DISCONNECTED
#define MEDUSA_HTTPREQUEST_STATE_RESOLVING              MEDUSA_HTTPREQUEST_STATE_RESOLVING
#define MEDUSA_HTTPREQUEST_STATE_RESOLVED               MEDUSA_HTTPREQUEST_STATE_RESOLVED
#define MEDUSA_HTTPREQUEST_STATE_CONNECTING             MEDUSA_HTTPREQUEST_STATE_CONNECTING
#define MEDUSA_HTTPREQUEST_STATE_CONNECTED              MEDUSA_HTTPREQUEST_STATE_CONNECTED
#define MEDUSA_HTTPREQUEST_STATE_REQUESTING             MEDUSA_HTTPREQUEST_STATE_REQUESTING
#define MEDUSA_HTTPREQUEST_STATE_REQUESTED              MEDUSA_HTTPREQUEST_STATE_REQUESTED
#define MEDUSA_HTTPREQUEST_STATE_RECEIVING              MEDUSA_HTTPREQUEST_STATE_RECEIVING
#define MEDUSA_HTTPREQUEST_STATE_RECEIVED               MEDUSA_HTTPREQUEST_STATE_RECEIVED
};

struct medusa_httprequest_init_options {
        struct medusa_monitor *monitor;
        int (*onevent) (struct medusa_httprequest *httprequest, unsigned int events, void *context, ...);
        void *context;
};

#ifdef __cplusplus
extern "C"
{
#endif

int medusa_httprequest_init_options_default (struct medusa_httprequest_init_options *options);

struct medusa_httprequest * medusa_httprequest_create (struct medusa_monitor *monitor, int (*onevent) (struct medusa_httprequest *httprequest, unsigned int events, void *context, ...), void *context);
struct medusa_httprequest * medusa_httprequest_create_with_options (const struct medusa_httprequest_init_options *options);
void medusa_httprequest_destroy (struct medusa_httprequest *httprequest);

unsigned int medusa_httprequest_get_state (const struct medusa_httprequest *httprequest);

int medusa_httprequest_set_connect_timeout (struct medusa_httprequest *httprequest, double timeout);
double medusa_httprequest_get_connect_timeout (const struct medusa_httprequest *httprequest);

int medusa_httprequest_add_header (struct medusa_httprequest *httprequest, const char *key, const char *value, ...) __attribute__((format(printf, 3, 4)));
int medusa_httprequest_add_vheader (struct medusa_httprequest *httprequest, const char *key, const char *value, va_list va);

int medusa_httprequest_make_post (struct medusa_httprequest *httprequest, const char *url, const void *data, int64_t length);
int medusa_httprequest_make_post_string (struct medusa_httprequest *httprequest, const char *url, const char *data);

int medusa_httprequest_onevent (struct medusa_httprequest *httprequest, unsigned int events);
struct medusa_monitor * medusa_httprequest_get_monitor (struct medusa_httprequest *httprequest);

const struct medusa_httprequest_reply * medusa_httprequest_get_reply (const struct medusa_httprequest *httprequest);

const struct medusa_httprequest_reply_status * medusa_httprequest_reply_get_status (const struct medusa_httprequest_reply *reply);
int64_t medusa_httprequest_reply_status_get_code (const struct medusa_httprequest_reply_status *status);
const char * medusa_httprequest_reply_status_get_value (const struct medusa_httprequest_reply_status *status);

const struct medusa_httprequest_reply_headers * medusa_httprequest_reply_get_headers (const struct medusa_httprequest_reply *reply);
int64_t medusa_httprequest_reply_headers_get_count (const struct medusa_httprequest_reply_headers *headers);
const struct medusa_httprequest_reply_header * medusa_httprequest_reply_headers_get_first (const struct medusa_httprequest_reply_headers *headers);

const char * medusa_httprequest_reply_header_get_key (const struct medusa_httprequest_reply_header *header);
const char * medusa_httprequest_reply_header_get_value (const struct medusa_httprequest_reply_header *header);
const struct medusa_httprequest_reply_header * medusa_httprequest_reply_header_get_next (const struct medusa_httprequest_reply_header *header);

const struct medusa_httprequest_reply_body * medusa_httprequest_reply_get_body (const struct medusa_httprequest_reply *reply);
int64_t medusa_httprequest_reply_body_get_length (const struct medusa_httprequest_reply_body *body);
const void * medusa_httprequest_reply_body_get_value (const struct medusa_httprequest_reply_body *body);

const char * medusa_httprequest_event_string (unsigned int events);

#ifdef __cplusplus
}
#endif

#endif
