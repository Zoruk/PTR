XENOCONFIG=/usr/bin/xeno-config
CC=	$(shell $(XENOCONFIG) --cc)
CFLAGS= $(shell $(XENOCONFIG) --skin=native --cflags)
LDFLAGS=$(shell $(XENOCONFIG) --skin=native --ldflags)
LDFLAGS+=-lnative
LDLIBS=-lnative -lxenomai

all: intro_watchdog

intro_watchdog: intro_watchdog.o busycpu.o watchdog.o
	$(CC) -o intro_watchdog intro_watchdog.o busycpu.o watchdog.o $(LDFLAGS) $(LDLIBS)

intro_watchdog.o: intro_watchdog.c busycpu.h watchdog.h general.h
	$(CC) -o intro_watchdog.o -c intro_watchdog.c $(CFLAGS)

watchdog.o: watchdog.c busycpu.h general.h
	$(CC) -o watchdog.o -c watchdog.c $(CFLAGS)

busycpu.o: busycpu.c
	$(CC) -o busycpu.o -c busycpu.c $(CFLAGS)

clean:
	rm -f *.o intro_watchdog 

.PHONY: clean

