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

#include <windows.h>
#include <time.h>
#include <unistd.h>

#include "battery.h"
#include "cmdline.h"

int monitor_battery(FILE *fp, HANDLE subprocess)
{
    int ret = EXIT_FAILURE;
    struct battery *b = get_first_battery();
    if (!b)
        goto done;

    time_t start = time(NULL);

    battery_write_header(fp, b);
    while (true) {
        if (!battery_write_status(fp, b, start))
            break;

        if (subprocess) {
            switch (WaitForSingleObject(subprocess, 0)) {
            case WAIT_TIMEOUT:
                break;
            case WAIT_OBJECT_0:
                ret = EXIT_SUCCESS;
                goto done;
            default:
                printf("ERROR: Unhandled return value from WaitForSingleObject\n");
                goto done;
            }
        }

        sleep(2);
    }

done:
    battery_destroy(b);
    return ret;
}

int main(int argc,char* argv[])
{
    if (argc <= 1)
        return monitor_battery(stdout, NULL);

    int ret = EXIT_FAILURE;
    char *cmdline = NULL;
    char *filename = NULL;
    FILE *fp    = NULL;

    PROCESS_INFORMATION ProcessInfo = {0};
    STARTUPINFO StartupInfo = {0};
    StartupInfo.cb = sizeof(StartupInfo);

    cmdline  = cmdline_build(argc, argv, CMDLINE_CREATE_PROCESS);
    if (!cmdline)
        goto done;

    filename = cmdline_build(argc, argv, CMDLINE_LOG_FILENAME);
    if (!filename)
        goto done;

    fix_log_filename(filename);

    if (!CreateProcessA(NULL, cmdline, NULL,
                        NULL,FALSE,0,NULL, NULL,
                        &StartupInfo,&ProcessInfo)) {
        printf("ERROR: Failed to spawn process\n");
        goto done;
    }
    CloseHandle(ProcessInfo.hThread);
    printf("filename = %s\n", filename);
    fp = fopen(filename, "w");
    if (!fp) {
        printf("ERROR: Failed to open output file %s\n", filename);
        goto done;
    }

    ret = monitor_battery(fp, ProcessInfo.hProcess);
done:
    if (fp)
        fclose(fp);

    free(cmdline);
    free(filename);

    if (ProcessInfo.hProcess)
        CloseHandle(ProcessInfo.hProcess);

    return ret;
}
