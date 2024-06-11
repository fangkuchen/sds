default: build

build: clean
	cp sensors.def.h sensors.h
	gcc -Wall -o mts mts.c -l curl

install: build
	cp -f mts /usr/bin/mts
	chmod +x /usr/bin/mts

uninstall:
	rm -f /usr/bin/mts

clean:
	rm -rf mts
