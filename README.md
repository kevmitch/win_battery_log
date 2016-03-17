## win_battery_log

This is a small command line utility to measure and record the battery
statistics in Microsoft Windows. It should report the battery charge or
discharge rate in milliwatts, the voltage in millivolts and for each
measurement, the number of seconds since recording began. It was written to
profile the power consumption of [mpv media
player](https://github.com/mpv-player/mpv) using various hardware decoding
scenarios, but may be useful for development of other applications for which
battery conservation is important.

## Usage

With no arguments, it simply prints these statistics to the console every two
seconds until interrupted.

    win_battery_log.exe

It can also be used to measure the battery statistics while running a given
command by simply adding that command and its argumets as arguments to the
win_battery_log executable. For example:

    win_battery_log.exe C:\windows\notepad.exe textfile.txt

win_battery_log will open a log file named by sanitizing the given commandline
into a file-friendly string by replacing special characters with "-" and spaces
with "_" and appending the Unix time in seconds followed by the ".log" suffix.
It will write battery statistics to this file until the command exits.

## Compilation

win_battery_log can be compiled using mingw-w64 in the
[MSYS2](https://msys2.github.io/) environment, or as a
[Cygwin](https://www.cygwin.com/) program. In the latter case however, the path
to the executable used as its second argument must be in Windows, as opposed to
Unix format.

Other than standard Windows libraries, win_battery_log should only depend on a
functioning c compiler (it has only been tested with gcc) and make. To build it,
from inside the project root simply type

    make

To remove the resulting executable and object files, as usual, it is sufficient
to type

    make clean
