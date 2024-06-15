default: build

build: clean
	@if [ ! -f sensors.h ]; then cp sensors.def.h sensors.h; fi
	gcc -Wall -o sds sds.c -l curl

install: build
	cp -f mts /usr/bin/sds
	chmod +x /usr/bin/sds

uninstall:
	rm -f /usr/bin/sds

clean:
	rm -rf sds
