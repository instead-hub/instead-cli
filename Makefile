include Makefile.common

LUA_CFLAGS=$(shell pkg-config --cflags luajit)
LUA_LFLAGS=$(shell pkg-config --libs luajit)

CFLAGS	+= -O2 -Wall -Dunix -D_HAVE_ICONV

EXE=
PLATFORM=unix.c
RM=rm
