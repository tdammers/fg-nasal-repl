EXECUTABLE=nasali
CC=gcc
LD=gcc
RM=rm -f
CFLAGS=-O2 -g -pedantic -Wall -Werror

PKG_CONFIG_DEPS=readline

CFLAGS+=$(shell pkg-config --cflags $(PKG_CONFIG_DEPS))
LOADLIBES=$(shell pkg-config --libs $(PKG_CONFIG_DEPS))

HEADERS=src/*.h
INCLUDEDIR=src
SOURCES=$(wildcard src/*.c)
OBJECTS=$(SOURCES:src/%.c=build/%.o)
CFLAGS+=-I $(INCLUDEDIR)

.PHONY: all
all: nasali

.PHONY: clean
clean:
	$(RM) build/*.o

nasali: $(OBJECTS)
	$(LD) $(LDFLAGS) $^ $(LOADLIBES) -o $@

build/%.o: src/%.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

build/main.o build/fgfsclient.o: src/fgfsclient.h
