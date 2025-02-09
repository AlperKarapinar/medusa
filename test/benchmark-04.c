
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <getopt.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <time.h>
#include <signal.h>

#include "medusa/error.h"
#include "medusa/clock.h"
#include "medusa/io.h"
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

static int g_backend;
static unsigned int g_nloops;
static unsigned int g_nsamples;
static unsigned int g_ntimers;
static unsigned int g_npipes;
static unsigned int g_nactives;
static unsigned int g_nwrites;
static unsigned int g_fired;
static unsigned int g_count;
static unsigned int g_writes;
static unsigned int g_failures;

static int *g_pipes;
static struct medusa_io **g_ios;
static struct medusa_timer **g_timers;

static int timer_onevent (struct medusa_timer *timer, unsigned int events, void *context, ...)
{
        (void) timer;
        (void) events;
        (void) context;
        return 0;
}

static int io_onevent (struct medusa_io *io, unsigned int events, void *context, ...)
{
        int rc;
        uintptr_t id;
        unsigned int wid;
        unsigned char ch;
        ssize_t n;

        id = (uintptr_t) context;
        wid = id + 1;

        if (events & MEDUSA_IO_EVENT_IN) {
                if (g_ntimers) {
                        rc = medusa_timer_set_interval(g_timers[id], 10 + drand48());
                        if (rc < 0) {
                                return -1;
                        }
                }
                n = read(medusa_io_get_fd(io), (char *) &ch, sizeof(ch));
                if (n >= 0) {
                        g_count += 1;
                } else {
                        g_failures++;
                }
                if (g_writes) {
                        if (wid >= g_npipes) {
                                wid -= g_npipes;
                        }
                        n = write(g_pipes[2 * wid + 1], "e", 1);
                        if (n != 1) {
                                g_failures++;
                        }
                        g_writes--;
                        g_fired++;
                }
        }

        return 0;
}

static int test_poll (unsigned int poll)
{
        int rc;
        unsigned int i;
        unsigned int j;
        unsigned int k;
        unsigned int space;

        struct medusa_monitor *monitor;
        struct medusa_monitor_init_options options;

        struct timeval create_start;
        struct timeval create_finish;
        struct timeval create_total;
        struct timeval destroy_start;
        struct timeval destroy_finish;
        struct timeval destroy_total;
        struct timeval apply_start;
        struct timeval apply_finish;
        struct timeval apply_total;
        struct timeval run_start;
        struct timeval run_finish;
        struct timeval run_total;

        timerclear(&create_total);
        timerclear(&destroy_total);
        timerclear(&apply_total);
        timerclear(&run_total);

        timerclear(&create_start);
        timerclear(&destroy_start);
        timerclear(&apply_start);
        timerclear(&run_start);

        timerclear(&create_finish);
        timerclear(&destroy_finish);
        timerclear(&apply_finish);
        timerclear(&run_finish);

        monitor = NULL;

        medusa_monitor_init_options_default(&options);
        options.poll.type = poll;

        for (j = 0; j < g_nloops; j++) {
                gettimeofday(&create_start, NULL);
                monitor = medusa_monitor_create(&options);
                if (monitor == NULL) {
                        goto bail;
                }
                for (i = 0; i < g_npipes; i++) {
                        g_ios[i] = medusa_io_create(monitor, g_pipes[i * 2], io_onevent, (void *) ((uintptr_t) i));
                        if (MEDUSA_IS_ERR_OR_NULL(g_ios[i])) {
                                goto bail;
                        }

                        if (g_ntimers) {
                                g_timers[i] = medusa_timer_create(monitor, timer_onevent, (void *) ((uintptr_t) i));
                                if (MEDUSA_IS_ERR_OR_NULL(g_timers[i])) {
                                        goto bail;
                                }
                        }
                }
                rc = medusa_monitor_run_timeout(monitor, 0.0);
                if (rc < 0) {
                        goto bail;
                }
                gettimeofday(&create_finish, NULL);
                timersub(&create_finish, &create_start, &create_finish);
                timeradd(&create_finish, &create_total, &create_total);

                for (k = 0; k < g_nsamples; k++) {
                        gettimeofday(&apply_start, NULL);
                        for (i = 0; i < g_npipes; i++) {
                                rc = 0;
                                if (medusa_io_get_enabled(g_ios[i])) {
                                        rc |= medusa_io_set_enabled(g_ios[i], 0);
                                }
                                rc |= medusa_io_set_events(g_ios[i], MEDUSA_IO_EVENT_IN);
                                rc |= medusa_io_set_enabled(g_ios[i], 1);
                                if (rc < 0) {
                                        goto bail;
                                }

                                if (g_ntimers) {
                                        if (medusa_timer_get_enabled(g_timers[i])) {
                                                rc |= medusa_timer_set_enabled(g_timers[i], 0);
                                        }
                                        rc |= medusa_timer_set_interval(g_timers[i], 10.0 + drand48());
                                        rc |= medusa_timer_set_enabled(g_timers[i], 1);
                                        if (rc != 0) {
                                                goto bail;
                                        }
                                }
                        }
                        rc = medusa_monitor_run_timeout(monitor, 0.0);
                        if (rc < 0) {
                                goto bail;
                        }
                        gettimeofday(&apply_finish, NULL);
                        timersub(&apply_finish, &apply_start, &apply_finish);
                        timeradd(&apply_finish, &apply_total, &apply_total);

                        g_fired = 0;
                        space = g_npipes / g_nactives;
                        space = space * 2;
                        for (i = 0; i < g_nactives; i++, g_fired++) {
                                (void) write(g_pipes[i * space + 1], "e", 1);
                        }

                        g_count = 0;
                        g_writes = g_nwrites;

                        gettimeofday(&run_start, NULL);
                        do {
                                rc = medusa_monitor_run_timeout(monitor, 0.0);
                                if (rc < 0) {
                                        goto bail;
                                }
                        } while (g_count != g_fired);
                        gettimeofday(&run_finish, NULL);
                        timersub(&run_finish, &run_start, &run_finish);
                        timeradd(&run_finish, &run_total, &run_total);

                        fprintf(stderr, "%8ld %8ld %8ld %8ld\n",
                                        create_finish.tv_sec * 1000000 + create_finish.tv_usec,
                                        apply_finish.tv_sec * 1000000 + apply_finish.tv_usec,
                                        run_finish.tv_sec * 1000000 + run_finish.tv_usec,
                                        destroy_finish.tv_sec * 1000000 + destroy_finish.tv_usec);
                }
                gettimeofday(&destroy_start, NULL);
                medusa_monitor_destroy(monitor);
                gettimeofday(&destroy_finish, NULL);
                timersub(&destroy_finish, &destroy_start, &destroy_finish);
                timeradd(&destroy_finish, &destroy_total, &destroy_total);
        }

        fprintf(stderr, "%8ld %8ld %8ld %8ld %8ld\n",
                        create_total.tv_sec * 1000000 + create_total.tv_usec,
                        apply_total.tv_sec * 1000000 + apply_total.tv_usec,
                        run_total.tv_sec * 1000000 + run_total.tv_usec,
                        destroy_total.tv_sec * 1000000 + destroy_total.tv_usec,
                        (create_total.tv_sec * 1000000 + create_total.tv_usec) +
                        (apply_total.tv_sec * 1000000 + apply_total.tv_usec) +
                        (run_total.tv_sec * 1000000 + run_total.tv_usec) +
                        (destroy_total.tv_sec * 1000000 + destroy_total.tv_usec));

        fprintf(stderr, "%8ld %8ld %8ld %8ld %8ld\n",
                        (create_total.tv_sec * 1000000 + create_total.tv_usec) / g_nloops,
                        (apply_total.tv_sec * 1000000 + apply_total.tv_usec) / (g_nloops * g_nsamples),
                        (run_total.tv_sec * 1000000 + run_total.tv_usec) / (g_nloops * g_nsamples),
                        (destroy_total.tv_sec * 1000000 + destroy_total.tv_usec) / g_nloops,
                        ((create_total.tv_sec * 1000000 + create_total.tv_usec) / g_nloops) +
                        ((apply_total.tv_sec * 1000000 + apply_total.tv_usec) / (g_nloops * g_nsamples)) +
                        ((run_total.tv_sec * 1000000 + run_total.tv_usec) / (g_nloops * g_nsamples)) +
                        ((destroy_total.tv_sec * 1000000 + destroy_total.tv_usec) / g_nloops));

        return 0;
bail:   if (monitor != NULL) {
                medusa_monitor_destroy(monitor);
        }
        return -1;
}

static void alarm_handler (int sig)
{
        (void) sig;
        abort();
}

int main (int argc, char *argv[])
{
        int c;
        int rc;
        unsigned int i;

        srand(time(NULL));
        signal(SIGALRM, alarm_handler);

        g_backend  = -1;
        g_nloops   = 10;
        g_nsamples = 10;
        g_pipes    = NULL;
        g_npipes   = 10;
        g_nactives = 2;
        g_nwrites  = g_npipes;
        g_ntimers  = 0;

        while ((c = getopt(argc, argv, "hb:l:s:n:a:w:t:")) != -1) {
                switch (c) {
                        case 'b':
                                g_backend = atoi(optarg);
                                break;
                        case 'l':
                                g_nloops = atoi(optarg);
                                break;
                        case 's':
                                g_nsamples = atoi(optarg);
                                break;
                        case 'n':
                                g_npipes = atoi(optarg);
                                break;
                        case 'a':
                                g_nactives = atoi(optarg);
                                break;
                        case 'w':
                                g_nwrites = atoi(optarg);
                                break;
                        case 't':
                                g_ntimers = !!atoi(optarg);
                                break;
                        case 'h':
                                fprintf(stderr, "%s [-b backend] [-l loops] [-s samples] [-n pipes] [-a actives] [-w writes] [-t timers]\n", argv[0]);
                                fprintf(stderr, "  -b: poll backend (default: %d)\n", g_backend);
                                fprintf(stderr, "  -l: loop count (default: %d)\n", g_nloops);
                                fprintf(stderr, "  -s: sample count (default: %d)\n", g_nsamples);
                                fprintf(stderr, "  -n: number of pipes (default: %d)\n", g_npipes);
                                fprintf(stderr, "  -a: number of actives (default: %d)\n", g_nactives);
                                fprintf(stderr, "  -w: number of writes (default: %d)\n", g_nwrites);
                                fprintf(stderr, "  -t: enable timers (default: %d)\n", g_ntimers);
                                return 0;
                        default:
                                fprintf(stderr, "unknown param: %c\n", c);
                                return -1;
                }
        }

        fprintf(stderr, "backend : %d\n", g_backend);
        fprintf(stderr, "loops   : %d\n", g_nloops);
        fprintf(stderr, "samples : %d\n", g_nsamples);
        fprintf(stderr, "pipes   : %d\n", g_npipes);
        fprintf(stderr, "actives : %d\n", g_nactives);
        fprintf(stderr, "writes  : %d\n", g_nwrites);
        fprintf(stderr, "timers  : %d\n", g_ntimers);

        g_ios = malloc(sizeof(struct medusa_io *) * g_npipes);
        if (g_ios == NULL) {
                return -1;
        }
        g_timers = malloc(sizeof(struct medusa_timer *) * g_npipes);
        if (g_timers == NULL) {
                return -1;
        }
        g_pipes = malloc(sizeof(int[2]) * g_npipes);
        if (g_pipes == NULL) {
                return -1;
        }
        for (i = 0; i < g_npipes; i++) {
#if 0
                rc = pipe(&g_pipes[i * 2]);
#else
                rc = socketpair(AF_UNIX, SOCK_STREAM, 0, &g_pipes[i * 2]);
#endif
                if (rc != 0) {
                        fprintf(stderr, "can not create pair: %d\n", i);
                        return -1;
                }
        }

        if (g_backend >= 0) {
                fprintf(stderr, "testing poll: %d ...\n", g_backend);

                rc = test_poll(g_backend);
                if (rc != 0) {
                        fprintf(stderr, "fail\n");
                        return -1;
                } else {
                        fprintf(stderr, "success\n");
                }
        } else {
                for (i = 0; i < sizeof(g_polls) / sizeof(g_polls[0]); i++) {
                        fprintf(stderr, "testing poll: %d ...\n", g_polls[i]);

                        rc = test_poll(g_polls[i]);
                        if (rc != 0) {
                                fprintf(stderr, "fail\n");
                                return -1;
                        } else {
                                fprintf(stderr, "success\n");
                        }
                }
        }

        for (i = 0; i < g_npipes; i++) {
                close(g_pipes[i * 2]);
                close(g_pipes[i * 2 + 1]);
        }
        free(g_pipes);
        free(g_timers);
        free(g_ios);
        return 0;
}
