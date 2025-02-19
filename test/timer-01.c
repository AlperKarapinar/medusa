
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <errno.h>

#include "medusa/error.h"
#include "medusa/timer.h"
#include "medusa/monitor.h"

static const unsigned int g_polls[] = {
        MEDUSA_MONITOR_POLL_DEFAULT,
#if defined(__LINUX__)
        MEDUSA_MONITOR_POLL_EPOLL,
#endif
#if defined(__APPLE__)
        MEDUSA_MONITOR_POLL_KQUEUE,
#endif
        MEDUSA_MONITOR_POLL_POLL,
        MEDUSA_MONITOR_POLL_SELECT
};

static int timer_onevent (struct medusa_timer *timer, unsigned int events, void *context, ...)
{
        unsigned int *tevents = (unsigned int *) context;
        (void) timer;
        *tevents |= events;
        return 0;
}

static int test_poll (unsigned int poll)
{
        int rc;

        struct medusa_monitor *monitor;
        struct medusa_monitor_init_options options;

        unsigned int tevents;
        struct medusa_timer *timer;

        monitor = NULL;

        medusa_monitor_init_options_default(&options);
        options.poll.type = poll;

        monitor = medusa_monitor_create(&options);
        if (monitor == NULL) {
                goto bail;
        }

        tevents = 0;
        timer = medusa_timer_create(monitor, timer_onevent, &tevents);
        if (MEDUSA_IS_ERR_OR_NULL(timer)) {
                goto bail;
        }
        rc = medusa_timer_set_initial(timer, 1.0);
        if (rc < 0) {
                goto bail;
        }
        rc = medusa_timer_set_interval(timer, 1.0);
        if (rc < 0) {
                goto bail;
        }
        rc = medusa_timer_set_singleshot(timer, 1);
        if (rc < 0) {
                goto bail;
        }
        rc = medusa_timer_set_resolution(timer, MEDUSA_TIMER_RESOLUTION_NANOSECOMDS);
        if (rc < 0) {
                goto bail;
        }
        rc = medusa_timer_set_enabled(timer, 1);
        if (rc < 0) {
                goto bail;
        }

        medusa_monitor_destroy(monitor);
        monitor = NULL;

        if (tevents != (MEDUSA_TIMER_EVENT_DESTROY)) {
                fprintf(stderr, "tevents: 0x%08x is invalid\n", tevents);
                goto bail;
        }
        return 0;
bail:   if (monitor != NULL) {
                medusa_monitor_destroy(monitor);
        }
        return -1;
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
