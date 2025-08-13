# instead-cli

Trivial INSTEAD interpreter for developers.

## Build

Dependencies: luajit (or lua), iconv.

```bash
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

Edit `Makefile.defs`.

```bash
DESTDIR=/
BIN=/usr/local/bin
STEADPATH=/usr/local/share/instead-cli/
```

Then:

```bash
$ make && sudo make install
```

### 9front (Plan9) build

```bash
% git/clone https://github.com/instead-hub/instead-cli.git
% mk init
% mk
% mk install
```

## Run

```bash
./instead-cli <gamedir path>
```

To pass internal command to STEAD use `/` prefix. Some internal commands:

* `/act 1` (or just `1`)

  ```bash
  $ instead-cli fantasy-game
  Room

  The lever(1) on the wall is raised.

  > /act 1
  The lever(1) on the wall is lowered.

  > 1
  The lever(1) on the wall is raised.

  >
  ```

* `/use 1,2` (or just `1,2`)

  ```bash
  $ instead-cli apple-game
  Room

  The apple(1) is lying on the floor. The table(2) stands in the
  corner.

  > 1
  /I take the apple./

  The table(1) stands in the corner.

  ** Apple(2)

  > 2,1
  The table(1) stands in the corner. The apple(2) lies on the table.

  >
  ```

* `/go 1` (or just `1`)

  ```bash
  $ instead-cli game/
  Room

  The lever(1) on the wall is raised.

  >

  >> Kitchen(2)

  > /go 2
  Kitchen

  The apple(1) is lying on the floor. The table(2) stands in the
  corner.

  >

  >> Room(3)

  > 3
  Room

  The lever(1) on the wall is raised.

  >> Kitchen(2)
  ```

* `/way` (or just empty line) — return list of ways from current room
* `/save filename`
* `/load filename`
* `/quit`
* `/inv` (or just empty line) — return items in the inventory
* `/look` — return the description of the current room

Options:

* `-d` - debug mode;
* `-dfile` - debug mode + write stderr to file;
* `-wnum` - line width is num symbols (70 by default);
* `-iscript` - read script line by line and use it as commands;
* `-lfile` - write all input commands to file;
* `-cpCODEPAGE` - Win only (1251 by default), use 65001 for UTF-8;
* `-a` - autosave on exit and autoload on start (autosave file);
* `-x` - execute lua script;
* `-e` - echo input command;
* `-m` - enable multimedia output;
* `-mcmd` - enable run cmd on multimedia. Examples: `-m/usr/bin/xdg-open` (Linux), `-m/bin/plumb` (Plan9), `-m"start\"\""` (Windows).

## Links

* https://instead.hugeping.ru
* https://parser.hugeping.ru
* http://instead-games.ru
