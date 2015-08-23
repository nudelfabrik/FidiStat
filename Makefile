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
LIB_DIR=/usr/local
CFLAGS=-I $(LIB_DIR)/include -g
INS_DIR=/usr/local/etc

all: fidistat

fidistat: main.o client.o server.o config.o json.o bootstrap.o tls.o 
	$(CC) $(CFLAGS) -lutil -L $(LIB_DIR)/lib main.o client.o server.o config.o json.o bootstrap.o tls.o -o fidistat -lconfig -ljansson -ltls

main.o: main.c
	$(CC) $(CFLAGS) -c main.c 

client.o: client.c
	$(CC) $(CFLAGS) -c client.c

server.o: server.c
	$(CC) $(CFLAGS) -c server.c

config.o: config.c 
	@if [ ! -f $(LIB_DIR)/lib/libconfig.a ]; then \
		if [ -d /usr/ports/devel/libconfig ]; then \
			cd /usr/ports/devel/libconfig && make install clean ; \
		else \
			echo "libconfig not installed and not found in /usr/ports/" ; \
			echo "Refer to README to install manually"; \
		fi; \
	fi
	@echo "config installed"
	$(CC) $(CFLAGS) -c config.c

json.o: json.c
	@if [ ! -f $(LIB_DIR)/lib/libjansson.a  ]; then \
		if [ -d /usr/ports/devel/jansson ]; then \ cd /usr/ports/devel/jansson && make install clean ; \
		else \
			echo "jansson not installed and not found in /usr/ports/" ; \
			echo "Refer to README to install manually"; \
		fi ; \
	fi
	@echo "jansson installed"
	$(CC) $(CFLAGS) -c json.c 

bootstrap.o: bootstrap.c
	$(CC) $(CFLAGS) -c bootstrap.c

tls.o: tls.c
	$(CC) $(CFLAGS) -c tls.c

install: all
	install -m 0755 fidistat $(LIB_DIR)/bin
	test -d $(INS_DIR)/fidistat || mkdir $(INS_DIR)/fidistat
	test -d $(INS_DIR)/fidistat/json || mkdir $(INS_DIR)/fidistat/json
	cp -n configFiles/* $(INS_DIR)/fidistat || :
	gzip -k fidistat.man
	mkdir -p /usr/local/share/man1 || :
	mv fidistat.man.gz /usr/local/share/man1/fidistat.1.gz

clean: 
	-rm *.o
	-rm fidistat
