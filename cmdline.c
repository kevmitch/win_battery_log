/*
 * This file is part of win_battery_log.
 *
 * win_battery_log is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * win_battery_log is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with win_battery_log.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <time.h>

#include "cmdline.h"

static int append_to_cmdline(char **pos, size_t rem, char *arg,
                             enum cmdline_build_mode mode, bool last)
{
    if (!arg)
        return 0;
    char format[] = "\"%s\" ";
    size_t fmtlen = strlen(format);

    switch(mode) {
    case CMDLINE_CREATE_PROCESS:
        fmtlen = strlen(strncpy(format, "\"%s\" ", fmtlen));
        break;
    case CMDLINE_LOG_FILENAME:
        fmtlen = strlen(strncpy(format, "%s_", fmtlen));
        break;
    default:
        assert(0);
    }

    if (last)
        format[fmtlen - 1]= '\0';

    int added = snprintf(*pos, rem, format, arg);
    assert(added <= rem);
    *pos += added;
    return added;
}

char *cmdline_build(int argc, char *argv[], enum cmdline_build_mode mode)
{
    if (argc <= 1)
        return NULL;

    size_t len = 0;
    for (int i = 1; i < argc; i++)
        // the string, quotes and space or null
        len += strlen(argv[i]) + 2 + 1;

    const size_t maxlen = 32768;
    if (maxlen < len) {
        fprintf(stderr, "ERROR: Command line too long: %z characters > %z\n", len, maxlen);
        return NULL;
    }

    char *suffix = NULL;
    if (mode == CMDLINE_LOG_FILENAME)
        len += asprintf(&suffix, "_%019lld.log", time(NULL));

    char *cmdline = calloc(len, sizeof(char));
    if (!cmdline) {
        fprintf(stderr, "ERROR: Out of memory allocating command line string\n");
        return NULL;
    }

    size_t rem = len;
    char *pos = cmdline;
    for (int i = 1; i < argc; i++)
        rem -= append_to_cmdline(&pos, rem, argv[i], mode, i == argc - 1);

    rem -= append_to_cmdline(&pos, rem, suffix, mode, true);

    return cmdline;
}

void fix_log_filename(char *pos)
{
    const char allowed[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890-_,.;";
    if (pos && pos[0] &&
        (pos[0] == '.' || pos[0] == '-'))
        pos[0] = '_';

    size_t rem = strlen(pos);
    while (true) {
        size_t offset = strspn(pos, allowed);
        if (rem <= offset)
            return;
        rem -= offset;
        pos += offset;
        pos[0] = '-';
    }
}
