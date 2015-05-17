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
CFLAGS=-I $(LIB_DIR)/include 
INS_DIR=/usr/local/etc

all: fidistat

fidistat: main.o client.o config.o json.o regex.o bootstrap.o
	$(CC) $(CFLAGS) -lutil -L $(LIB_DIR)/lib main.o client.o config.o json.o regex.o bootstrap.o -o fidistat -lconfig -ljansson

main.o: main.c
	$(CC) $(CFLAGS) -c main.c 

client.o: client.c
	$(CC) $(CFLAGS) -c cliemt.c

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

regex.o: regex.c
	$(CC) -c regex.c

bootstrap.o: bootstrap.c
	$(CC) $(CFLAGS) -c bootstrap.c

install: all
	install -m 0755 fidistat $(LIB_DIR)/bin
	test -d $(INS_DIR)/fidistat || mkdir $(INS_DIR)/fidistat
	test -d $(INS_DIR)/fidistat/json || mkdir $(INS_DIR)/fidistat/json
	cp -n configFiles/* $(INS_DIR)/fidistat || :
	gzip fidistat.man
	mv fidistat.man.gz /usr/local/share/man1/fidistat.1.gz

clean: 
	-rm *.o
	-rm fidistat
