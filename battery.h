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
#include <stdbool.h>
#include <SetupAPI.h>
#include <batclass.h>
#include <devguid.h>
struct battery {
    SP_DEVICE_INTERFACE_DATA did;
    PSP_DEVICE_INTERFACE_DETAIL_DATA pdidd;
    HANDLE hBattery;
    BATTERY_QUERY_INFORMATION bqi;
    BATTERY_INFORMATION bi;
    BATTERY_WAIT_STATUS bws;
    BATTERY_STATUS bs;
};

void battery_destroy(struct battery *b);
struct battery *get_first_battery();
void battery_write_header(FILE *fp, struct battery *b);
bool battery_write_status(FILE *fp, struct battery *b, time_t start);
