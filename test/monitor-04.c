
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

#include "medusa/io.h"
#include "medusa/timer.h"
#include "medusa/monitor.h"

static const unsigned int g_polls[] = {
        medusa_monitor_poll_default,
#if defined(__LINUX__)
        medusa_monitor_poll_epoll,
#endif
#if defined(__APPLE__)
        medusa_monitor_poll_kqueue,
#endif
        medusa_monitor_poll_poll,
        medusa_monitor_poll_select
};

static int test_poll (unsigned int poll)
{
        int rc;

        struct medusa_monitor *monitor;
        struct medusa_monitor_init_options options;

        struct medusa_io *io;
        struct medusa_timer *timer;

        monitor = NULL;

        memset(&options, 0, sizeof(struct medusa_monitor_init_options));
        options.poll.type = poll;

        monitor = medusa_monitor_create(&options);
        if (monitor == NULL) {
                goto bail;
        }

        io = medusa_io_create();
        if (io == NULL) {
                goto bail;
        }
        rc = medusa_monitor_add(monitor, (struct medusa_subject *) io);
        if (rc != 0) {
                goto bail;
        }

        timer = medusa_timer_create();
        if (timer == NULL) {
                goto bail;
        }
        rc = medusa_monitor_add(monitor, (struct medusa_subject *) timer);
        if (rc != 0) {
                goto bail;
        }

        medusa_monitor_destroy(monitor);
        return 0;
bail:   if (monitor != NULL) {
                medusa_monitor_destroy(monitor);
        }
        return 01;
}

int main (int argc, char *argv[])
{
        int rc;
        unsigned int i;
        (void) argc;
        (void) argv;
        for (i = 0; i < sizeof(g_polls) / sizeof(g_polls[0]); i++) {
                rc = test_poll(g_polls[i]);
                if (rc != 0) {
                        return -1;
                }
        }
        return 0;
}
