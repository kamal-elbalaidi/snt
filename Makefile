
CC = gcc
CFLAGS = -Wall `pkg-config --cflags vte-2.91`
LIBS = `pkg-config --libs vte-2.91`
Name = snt

all: $(Name)

$(Name): snt.c config.h util.h
	${CC} -O2 ${CFLAGS} snt.c -o $(Name) ${LIBS}

install: $(Name)
	cp -f $(Name) /usr/bin/
	cp -f $(Name).1 /usr/share/man/man1/
	cp -f $(Name).png /usr/share/icons/hicolor/512x512/apps
	cp -f $(Name).desktop /usr/share/applications

uninstall:
	rm -f /usr/bin/$(Name)
	rm -f /usr/share/man/man1/$(Name).1
	rm -f /usr/share/icons/hicolor/512x512/apps/$(Name).png
	rm -f /usr/share/applications/$(Name).desktop

clean:
	rm -f $(Name)
