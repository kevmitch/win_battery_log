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

enum cmdline_build_mode {
    CMDLINE_CREATE_PROCESS,
    CMDLINE_LOG_FILENAME,
};
char *cmdline_build(int argc, char *argv[], enum cmdline_build_mode mode);
void fix_log_filename(char *pos);
