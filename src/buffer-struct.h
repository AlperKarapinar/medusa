
#if !defined(MEDUSA_BUFFER_STRUCT_H)
#define MEDUSA_BUFFER_STRUCT_H

struct medusa_buffer {
        void *buffer;
        int length;
        int size;
};

int medusa_buffer_init (struct medusa_buffer *buffer);
void medusa_buffer_uninit (struct medusa_buffer *buffer);

#endif
