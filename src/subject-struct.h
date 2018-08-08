
enum {
        MEDUSA_SUBJECT_FLAG_ALLOC       = 0x00000001,
        MEDUSA_SUBJECT_FLAG_MOD         = 0x00000100,
        MEDUSA_SUBJECT_FLAG_DEL         = 0x00000200,
        MEDUSA_SUBJECT_FLAG_ROGUE       = 0x00000400,
        MEDUSA_SUBJECT_FLAG_POLL        = 0x00010000,
        MEDUSA_SUBJECT_FLAG_HEAP        = 0x00020000,
        MEDUSA_SUBJECT_TYPE_IO          = 0x01000000,
        MEDUSA_SUBJECT_TYPE_TIMER       = 0x02000000,
        MEDUSA_SUBJECT_TYPE_TCPSOCKET   = 0x04000000,
        MEDUSA_SUBJECT_TYPE_MASK        = 0xff000000
#define MEDUSA_SUBJECT_FLAG_ALLOC       MEDUSA_SUBJECT_FLAG_ALLOC
#define MEDUSA_SUBJECT_FLAG_MOD         MEDUSA_SUBJECT_FLAG_MOD
#define MEDUSA_SUBJECT_FLAG_DEL         MEDUSA_SUBJECT_FLAG_DEL
#define MEDUSA_SUBJECT_FLAG_ROGUE       MEDUSA_SUBJECT_FLAG_ROGUE
#define MEDUSA_SUBJECT_FLAG_POLL        MEDUSA_SUBJECT_FLAG_POLL
#define MEDUSA_SUBJECT_FLAG_HEAP        MEDUSA_SUBJECT_FLAG_HEAP
#define MEDUSA_SUBJECT_TYPE_IO          MEDUSA_SUBJECT_TYPE_IO
#define MEDUSA_SUBJECT_TYPE_TIMER       MEDUSA_SUBJECT_TYPE_TIMER
#define MEDUSA_SUBJECT_TYPE_TCPSOCKET   MEDUSA_SUBJECT_TYPE_TCPSOCKET
#define MEDUSA_SUBJECT_TYPE_MASK        MEDUSA_SUBJECT_TYPE_MASK
};

TAILQ_HEAD(medusa_subjects, medusa_subject);
struct medusa_subject {
        TAILQ_ENTRY(medusa_subject) subjects;
        unsigned int flags;
        struct medusa_monitor *monitor;
};
