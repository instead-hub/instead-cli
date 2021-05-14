# instead-cli

Trivial INSTEAD interpreter for developers.

## Build and run

Dependencies: luajit (or lua), iconv.

```
$ git clone https://github.com/instead-hub/instead-cli.git
$ cd instead-cli
$ git submodule init
$ git submodule update
# edit Makefile if needed
$ make install
$ cd release
$ ./instead-cli
```

System wide setup:

Edit Makefile.defs.

```
DESTDIR=/
BIN=/usr/local/bin
STEADPATH=/usr/local/share/instead-cli/
```

Then:

```
$ make && sudo make install
```

### 9front (Plan9) build

```
% git/clone https://github.com/instead-hub/instead-cli.git
% mk init
% mk
% mk install
```

## Run

./instead-cli <gamedir path>

To pass internal command to STEAD use '/' prefix. Some internal commands:

* /save filename
* /load filename
* /quit
* /inv
* /look

Options:

* -d - debug mode;
* -dfile - debug mode + write stderr to file;
* -wnum - line width is num symbols (70 by default);
* -iscript - read script line by line and use it as commands;
* -lfile - write all input commands to file;
* -cpCODEPAGE - Win only (1251 by default), use 65001 for UTF-8;
* -a - autosave on exit and autoload on start (autosave file);
* -x - execute lua script;
* -m - enable multimedia output;
* -mcmd - enable run cmd on multimedia (for ex. -m/usr/bin/xdg-open).

## Links

* https://instead.hugeping.ru
* https://parser.hugeping.ru
* http://instead-games.ru
