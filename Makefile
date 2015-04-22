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

fidistat: main.o config.o jansson.o regex.o bootstrap.o
	$(CC) $(CFLAGS) -L $(LIB_DIR)/lib main.o config.o jansson.o regex.o bootstrap.o -o fidistat -lconfig -ljansson

main.o: main.c
	$(CC) $(CFLAGS) -c main.c 

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

jansson.o: jansson.c
	@if [ ! -f $(LIB_DIR)/lib/libjansson.a  ]; then \
		if [ -d /usr/ports/devel/jansson ]; then \ cd /usr/ports/devel/jansson && make install clean ; \
		else \
			echo "jansson not installed and not found in /usr/ports/" ; \
			echo "Refer to README to install manually"; \
		fi ; \
	fi
	@echo "jansson installed"
	$(CC) $(CFLAGS) -c jansson.c 

regex.o: regex.c
	$(CC) -c regex.c

bootstrap.o: bootstrap.c
	$(CC) $(CFLAGS) -c bootstrap.c

install: all
	install -m 0755 fidistat $(LIB_DIR)/bin
	test -d $(INS_DIR)/fidistat || mkdir $(INS_DIR)/fidistat
	test -d $(INS_DIR)/fidistat/json || mkdir $(INS_DIR)/fidistat/json
	cp -n configFiles/* $(INS_DIR)/fidistat || :

clean: 
	-rm *.o
	-rm fidistat
