</$objtype/mkfile

CC=pcc
CFLAGS= -DVERSION="1.1" -D_POSIX_SOURCE -Ilua/src -DPLAN9 -DUNIX -DSTEAD_PATH="/sys/games/lib/instead/"
LIBS=lua/src/liblua.a

all: $O.instead
	
$LIBS: lua
	
lua:
	hget -o lua.tar.gz http://lua.org/ftp/lua-5.4.3.tar.gz && tar xf lua.tar.gz && cp mkfile.lua lua-5.4.3/src/mkfile && cd lua-5.4.3/src && mk && cd ../.. && mv lua-5.4.3 lua

init:V:
	rm -rf instead
	rm -rf metaparser
	rm -rf lua*
	git/clone https://github.com/instead-hub/instead.git
	git/clone https://github.com/instead-hub/metaparser.git
	
update:V:
	cd instead && git/pull && cd .. && cd metaparser && git/pull

OFILES=\
	instead/src/instead/cache.$O\
	instead/src/instead/idf.$O\
	instead/src/instead/instead.$O\
	instead/src/instead/lfs.$O\
	instead/src/instead/list.$O\
	instead/src/instead/tinymt32.$O\
	instead/src/instead/util.$O\
	main.$O

%.$O: %.c
	$CC $CFLAGS -c -o $target $stem.c

$O.instead: $OFILES
	$CC $CFLAGS -o $target $OFILES $LIBS

install:V:
	mkdir -p /sys/games/lib/instead
	dircp instead/stead /sys/games/lib/instead
	cp tiny.lua /sys/games/lib/instead
	cp instead/src/tiny/tiny2.lua /sys/games/lib/instead/stead2
	cp instead/src/tiny/tiny3.lua /sys/games/lib/instead/stead3
	mkdir -p /sys/games/lib/instead/stead3/morph
	dircp metaparser/morph /sys/games/lib/instead/stead3/morph
	mkdir -p /sys/games/lib/instead/stead3/parser
	dircp metaparser/parser /sys/games/lib/instead/stead3/parser
	cp $O.instead /$objtype/bin/instead

clean:V:
	rm -f $OFILES $O.instead
