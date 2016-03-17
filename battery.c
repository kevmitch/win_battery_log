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

#include <initguid.h>
#include <windows.h>
#include <time.h>
#include <devguid.h>

#include "battery.h"

void battery_destroy(struct battery *b)
{
    if (b) {
        if (b->hBattery)
            CloseHandle(b->hBattery);

        if (b->pdidd)
            LocalFree(b->pdidd);

        LocalFree(b);
    }
}

static struct battery *battery_create(HDEVINFO hdev, int device_number)
{
    struct battery *b = LocalAlloc(LPTR, sizeof(struct battery));
    b->did.cbSize = sizeof(b->did);

    if (!SetupDiEnumDeviceInterfaces(hdev,
                                     0,
                                     &GUID_DEVCLASS_BATTERY,
                                     device_number,
                                     &b->did)) {
        if (GetLastError() != ERROR_NO_MORE_ITEMS)
            fprintf(stderr, "ERROR getting battery device %d\n", device_number);
        goto fail;
    }

    DWORD cbRequired = 0;
    SetupDiGetDeviceInterfaceDetail(hdev, &b->did, 0, 0, &cbRequired, 0);
    if (ERROR_INSUFFICIENT_BUFFER != GetLastError()) {
        fprintf(stderr, "ERROR: Unexpected return value when getting buffer size\n");
        goto fail;
    }

    b->pdidd = LocalAlloc(LPTR, cbRequired);
    if (!b->pdidd) {
        fprintf(stderr, "ERROR: Falied to allocate pdidd\n");
        goto fail;
    }

    b->pdidd->cbSize = sizeof(*b->pdidd);
    if (!SetupDiGetDeviceInterfaceDetail(hdev, &b->did, b->pdidd, cbRequired,
                                         &cbRequired, 0)) {
        fprintf(stderr, "ERROR: Failed to SetupDiGetDeviceInterfaceDetail\n");
        goto fail;
    }

    // Enumerated a battery.  Ask it for information.
    b->hBattery = CreateFile(b->pdidd->DevicePath,
                             GENERIC_READ | GENERIC_WRITE,
                             FILE_SHARE_READ | FILE_SHARE_WRITE,
                             NULL,
                             OPEN_EXISTING,
                             FILE_ATTRIBUTE_NORMAL,
                             NULL);
    if (b->hBattery == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "ERROR: Got invalid handle value from CreateFile\n");
        goto fail;
    }

    // Ask the battery for its tag.

    DWORD dwWait = 0;
    DWORD dwOut;
    if (!DeviceIoControl(b->hBattery,
                         IOCTL_BATTERY_QUERY_TAG,
                         &dwWait,
                         sizeof(dwWait),
                         &b->bqi.BatteryTag,
                         sizeof(b->bqi.BatteryTag),
                         &dwOut,
                         NULL)
        || !b->bqi.BatteryTag) {
        fprintf(stderr, "ERROR: Failed DeviceIoControl Query Tag\n");
        goto fail;
    }

    // With the tag, you can query the battery info.
    b->bqi.InformationLevel = BatteryInformation;

    if (!DeviceIoControl(b->hBattery,
                         IOCTL_BATTERY_QUERY_INFORMATION,
                         &b->bqi,
                         sizeof(b->bqi),
                         &b->bi,
                         sizeof(b->bi),
                         &dwOut,
                         NULL))
    {
        fprintf(stderr, "ERROR: Failed DeviceIoControl query information\n");
        goto fail;
    }
    // Only non-UPS system batteries count
    if (!(b->bi.Capabilities & BATTERY_SYSTEM_BATTERY))
    {
        fprintf(stderr, "ERROR: Skipping UPS?\n");
        goto fail;
    }

    if (b->bi.Capabilities & BATTERY_IS_SHORT_TERM)
    {
        fprintf(stderr, "ERROR: Skipping short term Battery\n");;
        goto fail;
    }

    // Query the battery status.
    b->bws.BatteryTag = b->bqi.BatteryTag;

    return b;
fail:
    if (b)
        battery_destroy(b);
    return NULL;
}

struct battery *get_first_battery()
{
    HDEVINFO hdev = SetupDiGetClassDevs(&GUID_DEVCLASS_BATTERY, 0, 0,
                                        DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    if (INVALID_HANDLE_VALUE == hdev) {
        fprintf(stderr, "ERROR: Failed to get hardware device list\n");
        return NULL;
    }

    struct battery *b = NULL;
    for (int i = 0; !b; i++) {
        b = battery_create(hdev, i);
        if (ERROR_NO_MORE_ITEMS == GetLastError()) {
            fprintf(stderr, "ERROR: Found no battery\n");
            goto done;
        }
    }

done:
    SetupDiDestroyDeviceInfoList(hdev);
    return b;
}

void battery_write_header(FILE *fp, struct battery *b)
{
    if (b->bi.Capabilities & BATTERY_CAPACITY_RELATIVE)
        fprintf(fp, "time(s) capacity voltage rate\n");
    else
        fprintf(fp, "time(s) capacity(mWh) voltage(mV) rate(mW)\n");
}

bool battery_write_status(FILE *fp, struct battery *b, time_t start)
{
    DWORD dwOut;
    if (!DeviceIoControl(b->hBattery,
                         IOCTL_BATTERY_QUERY_STATUS,
                         &b->bws,
                         sizeof(b->bws),
                         &b->bs,
                         sizeof(b->bs),
                         &dwOut,
                         NULL)) {
        fprintf(stderr, "ERROR: Failed DeviceIoControl query status\n");
        return false;
    }

    fprintf(fp, "%llu %llu %llu %lld\n",
           (unsigned long long)time(NULL) - start,
           (unsigned long long)b->bs.Capacity,
           (unsigned long long)b->bs.Voltage,
           (long long)b->bs.Rate);
    fflush(fp);
    return true;
}
