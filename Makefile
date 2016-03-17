CFLAGS += -D_GNU_SOURCE
EXE = win_battery_log.exe
$(EXE): main.o cmdline.o battery.o
	$(CC) $^ -lsetupapi -o $@
clean:
	$(RM) $(EXE) *.o
