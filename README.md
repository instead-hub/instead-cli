# instead-cli

## Build and run

```
$ git clone https://github.com/instead-hub/instead-cli.git
$ git submodule init
$ git submodule update
# edit Makefile.defs if needed
$ make install
$ cd release
$ ./instead-cli
```

System wide setup:

Edit Makefile.defs

```
DESTDIR=/
BIN=/usr/local/bin
STEADPATH=/usr/local/share/instead-cli/
```

Then:

```
$ make && sudo make install
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
* -a - autosave on exit and autoload on start (autosave file)
