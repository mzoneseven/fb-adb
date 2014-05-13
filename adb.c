#include <stdlib.h>
#include <sys/types.h>
#include <ctype.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include "adb.h"
#include "child.h"
#include "util.h"
#include "argv.h"

void
adb_send_file(const char* local,
              const char* remote,
              const char* const* adb_args)
{
    SCOPED_RESLIST(rl_send_stub);

    struct child_start_info csi = {
        .flags = CHILD_MERGE_STDERR,
        .exename = "adb",
        .argv = argv_concat((const char*[]){"adb", NULL},
                            adb_args ?: empty_argv,
                            (const char*[]){"push", local, remote, NULL},
                            NULL),
    };
    struct child* adb = child_start(&csi);
    fdh_destroy(adb->fd[0]);

    char buf[512];
    size_t len = read_all(adb->fd[1]->fd, buf, sizeof (buf));
    fdh_destroy(adb->fd[1]);

    int status = child_wait(adb);
    if (!(WIFEXITED(status) && WEXITSTATUS(status) == 0)) {
        if (len == sizeof (buf))
            --len;

        while (len > 0 && isspace(buf[len - 1]))
            --len;

        buf[len] = '\0';

        char* epos = buf;
        while (*epos != '\0' && isspace(*epos))
            ++epos;

        if (strncmp(epos, "error: ", strlen("error: ")) == 0) {
            epos += strlen("error: ");
            char* e = strchr(epos, '\n');
            if (e) *e = '\0';
        }

        die(ECOMM, "adb error: %s", epos);
    }
}
