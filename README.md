## win_battery_log

This is a small command line utility to measure and record battery statistics in
Microsoft Windows. It should report the battery capacity in milliwatt hours, the
voltage in millivolts, charge or discharge rate in milliwatts, and for each
measurement, the number of seconds since recording began.

It was written to profile the power consumption of [mpv media
player](https://github.com/mpv-player/mpv) using various hardware video decoding
scenarios, but may be useful for development of other applications for which
battery conservation is important.

## Usage

With no arguments, win_battery_log simply prints these statistics to the console
every two seconds until interrupted. So the command

    win_battery_log

Will return output like

    time(s) capacity(mWh) voltage(mV) rate(mW)
    0 79700 12674 -12521
    2 79690 12640 -12437
    4 79690 12659 -12418
    6 79670 12673 -12394

win_battery_log can also be used to measure the battery statistics for the
running duration of any command. Simply add that command, and its arguments as
arguments to win_battery_log itself. In this case, the statistics are written to
a log file named by sanitizing the commandline into a file-friendly name
suffixed by the Unix time in seconds and the ".log" extension. win_battery_log
will automatically stop logging as soon as the command exits.

For example:

    win_battery_log c:\windows\notepad.exe c:\windows\win.ini

will create a file called with the ugly, but functional name
`c--windows-notepad.exe_c--windows-win.ini_0000000001458235621.log` with
contents similar to the output shown above.

## Compilation

win_battery_log can be compiled using mingw-w64 in the
[MSYS2](https://msys2.github.io/) environment, or as a
[Cygwin](https://www.cygwin.com/) program. In the latter case, the path
to the executable used as its second argument must still be in Windows (as
opposed to Unix) format.

Other than standard Windows libraries, win_battery_log should only depend on
make and a functioning c compiler (although it has only been tested with
gcc). To build it, from inside the project root simply type

    make

To remove the resulting executable and object files, as usual, it is sufficient
to type

    make clean

## Bugs

* It simply reports statistics on the first battery it finds. If you have
  multiple batteries and you want to know other batteries, some
  modifications to the source code may be necessary.

* It is possible that your battery does not actually provide voltage or
  discharge rate in comprehensible units. In MSDN terminology, the battery
  reports only "relative" units. This will be reflected by the absense of units
  in the output header. There's probably not more much that can be done about
  this.

* After the child process has ended, win_battery_log may take up to two seconds
  to return to the command line. This is because it only checks if the program
  is terminated every two seconds. It's no doubt possible to handle this better,
  but the added complexity is probably not worth it.
