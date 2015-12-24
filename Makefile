CC=gcc
SYSTEM != uname -s
.if $(SYSTEM) == FreeBSD
BSDversion != uname -r | grep -o '^[0-9]*'
.    if $(BSDversion) == 10 
CC   = clang
.    endif
.endif
.if $(SYSTEM) == Darwin
CC   = clang
.endif
CFLAGS=-Wall -Wextra -pedantic -I ${PREFIX}/include

all: fidistat

fidistat: main.o client.o server.o config.o json.o bootstrap.o tls.o 
	$(CC) $(CFLAGS) -lutil -L ${PREFIX}/lib main.o client.o server.o config.o json.o bootstrap.o tls.o -o fidistat -lconfig -ljansson -ltls

main.o: main.c
	$(CC) $(CFLAGS) -c main.c 

client.o: client.c
	$(CC) $(CFLAGS) -c client.c

server.o: server.c
	$(CC) $(CFLAGS) -c server.c

config.o: config.c 
	$(CC) $(CFLAGS) -c config.c

json.o: json.c
	$(CC) $(CFLAGS) -c json.c 

bootstrap.o: bootstrap.c
	$(CC) $(CFLAGS) -c bootstrap.c

tls.o: tls.c
	$(CC) $(CFLAGS) -c tls.c

install: all
	install -m 0755 fidistat ${PREFIX}/bin
	test -d ${PREFIX}/etc/fidistat || mkdir ${PREFIX}/etc/fidistat
	test -d ${PREFIX}/etc/fidistat/json || mkdir ${PREFIX}/etc/fidistat/json
	cp configFiles/config.cfg ${PREFIX}/etc/fidistat/config.cfg.sample || :
	gzip -k fidistat.man
	mkdir -p ${PREFIX}/share/man1 || :
	mv fidistat.man.gz ${PREFIX}/share/man1/fidistat.1.gz
	install -m 0755 fidistat.rc ${PREFIX}/etc/rc.d/fidistat
	install -m 0755 fidistat-server.rc ${PREFIX}/etc/rc.d/fidistat-server

clean: 
	-rm *.o
	-rm fidistat
