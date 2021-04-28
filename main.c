#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <locale.h>
#ifdef _WIN32
#include <windows.h>
#endif
#include "instead/src/instead/instead.h"

#define WIDTH 70

static int opt_log = 0;
static int opt_debug = 0;
static int opt_width = WIDTH;
static char *opt_autoload = NULL;

static int need_restart = 0;
static int need_load = 0;
static int need_save = 0;
static int parser_mode = 0;

static int luaB_menu(lua_State *L)
{
	const char *menu = luaL_optstring(L, 1, NULL);
	if (!menu)
		return 0;
	need_save = !strcmp(menu, "save");
	need_load = !strcmp(menu, "load");
	return 0;
}

static int luaB_restart(lua_State *L)
{
	need_restart = !lua_isboolean(L, 1) || lua_toboolean(L, 1);
	return 0;
}

static const luaL_Reg tiny_funcs[] = {
	{ "instead_restart", luaB_restart },
	{ "instead_menu", luaB_menu },
	{ NULL, NULL }
};

static int tiny_init(void)
{
	int rc;
	instead_api_register(tiny_funcs);
	rc = instead_loadfile(STEAD_PATH"tiny.lua");
	if (rc)
		return rc;
	return 0;
}
static struct instead_ext ext = {
	.init = tiny_init,
};

#define utf_cont(p) ((*(p) & 0xc0) == 0x80)

static int utf_ff(const char *s, const char *e)
{
	int l = 0;
	if (!s || !e)
		return 0;
	if (s > e)
		return 0;
	if ((*s & 0x80) == 0) /* ascii */
		return 1;
	l = 1;
	while (s < e && utf_cont(s + 1)) {
		s ++;
		l ++;
	}
	return l;
}

static void fmt(const char *str, int width)
{
	int l;
	const char *ptr = str;
	const char *eptr = ptr + strlen(str);
	const char *s = str;
	const char *last = NULL;
	char c;
	int w = 0;
	while ((l = utf_ff(ptr, eptr))) {
		c = *ptr;
		ptr += l;
		w ++;
		if (c == ' ' || c == '\t' || c == '\n')
			last = ptr;
		if ((width > 0 && w > width && last) || c == '\n') {
			while(s != last)
				putc(*s++, stdout);
			if (c != '\n')
				putc('\n', stdout);
			w = 0;
			last = s;
			continue;
		}
	}
	while(s != eptr)
		putc(*s++, stdout);
	putc('\n', stdout);
}

static char *trim(char *str)
{
	char *eptr = str + strlen(str);
	while ((*eptr == '\n' || *eptr == 0 || *eptr == ' ') && eptr != str) *eptr-- = 0;
	str += strspn(str, " \t\n\r");
	return str;
}

static void footer(void)
{
	char *p;
	p = instead_cmd("way", NULL);
	if (p && *p) {
		puts(">> "); fmt(trim(p), opt_width);
		free(p);
	}
	p = instead_cmd("inv", NULL);
	if (p && *p) {
		puts("** "); fmt(trim(p), opt_width);
		free(p);
	}
}

static void reopen_stderr(const char *fname)
{
	if (*fname && freopen(fname, "w", stderr) != stderr) {
		fprintf(stderr, "Error opening '%s': %s\n", fname, strerror(errno));
		exit(1);
	}
}

static char *get_input(void)
{
	static char input[256], *p;
	input[0] = 0;
	p = fgets(input, sizeof(input), stdin);
	if (p && *p) {
		p[strcspn(p, "\n\r")] = 0;
	}
	return p;
}

int main(int argc, const char **argv)
{
	int rc, i;
	char *str;
	const char *game = NULL;
	char cmd[256 + 64];
	setlocale(LC_ALL, "");
#ifdef _WIN32
	SetConsoleOutputCP(1251);
	SetConsoleCP(1251);
#endif
	for (i = 1; i < argc; i++) {
		if (!strncmp(argv[i], "-l", 2)) {
			opt_log = 1;
			reopen_stderr(argv[i] + 2);
		} else if (!strncmp(argv[i], "-d", 2)) {
			opt_debug = 1;
			reopen_stderr(argv[i] + 2);
		} else if (!strncmp(argv[i], "-w", 2)) {
			opt_width = atoi(argv[i] + 2);
		} else if (!strncmp(argv[i], "-i", 2)) {
			if (freopen(argv[i] + 2, "r", stdin) != stdin) {
				fprintf(stderr, "Error opening '%s': %s\n", argv[i] + 2, strerror(errno));
				exit(1);
			}
		} else if (!game) {
			game = argv[i];
		}
	}

	if (!game) {
		fprintf(stdout, "Usage: %s [-d] [-w<width>] <game>\n", argv[0]);
		exit(1);
	}

	if (instead_extension(&ext)) {
		fprintf(stderr, "Can't register tiny extension\n");
		exit(1);
	}

	if (!opt_debug && !opt_log)
		fclose(stderr);

	instead_set_debug(opt_debug);
restart:
	if (instead_init(game)) {
		fprintf(stdout, "Can not init game: %s\n", game);
		exit(1);
	}
	if (instead_load(NULL)) {
		fprintf(stdout, "Can not load game: %s\n", instead_err());
		exit(1);
	}
	if (opt_autoload) {
		snprintf(cmd, sizeof(cmd), "load %s", opt_autoload);
		printf("%s\n", cmd);
		str = instead_cmd(cmd, &rc);
	} else
		str = instead_cmd("look", &rc);
	if (!rc && str) {
		fmt(trim(str), opt_width);
		fflush(stdout);
	}
	free(str);
	if (opt_autoload) {
		free(opt_autoload);
		opt_autoload = NULL;
	}
	need_restart = need_load = need_save = 0;
	while (1) {
		char *p;
		printf("> "); fflush(stdout);
		p = get_input();
		if (!p)
			break;
		if (!strcmp(p, "/quit"))
			break;

		if (!strncmp(p, "/load ", 6) || !strncmp(p, "/save ", 6)) {
			rc = 1; str = NULL;
		} else {
			snprintf(cmd, sizeof(cmd), "use %s", p); /* try use */
			str = instead_cmd(cmd, &rc);
			if (rc) { /* try go */
				free(str);
				snprintf(cmd, sizeof(cmd), "go %s", p);
				str = instead_cmd(cmd, &rc);
			}
		}
		if (rc) { /* try act */
			free(str);
			snprintf(cmd, sizeof(cmd), "%s", p);
			str = instead_cmd(cmd, &rc);
		}
		if (rc && !str) { /* parser? */
			parser_mode = 1;
			snprintf(cmd, sizeof(cmd), "@metaparser \"%s\"", p);
			str = instead_cmd(cmd, NULL);
		}
		if (str) {
			fmt(trim(str), opt_width);
			fflush(stdout);
		}
		free(str);
		if (!rc && !parser_mode) /* no parser */
			footer();
		if (opt_log)
			fprintf(stderr, "%s\n", p);
		if (need_restart) {
			instead_done();
			goto restart;
		}
		if (need_save) {
			puts("?(autosave)"); fflush(stdout);
			p = get_input();
			if (p && *p)
				snprintf(cmd, sizeof(cmd), "save %s", p);
			else
				snprintf(cmd, sizeof(cmd), "save autosave");
			printf("%s\n", cmd);
			str = instead_cmd(cmd, NULL);
			if (str)
				free(str);
			need_save = 0;
		} else if (need_load) {
			puts("?(autosave)"); fflush(stdout);
			p = get_input();
			if (opt_autoload)
				free(opt_autoload);
			if (p && *p)
				opt_autoload = strdup(p);
			else
				opt_autoload = strdup("autosave");
			if (!access(opt_autoload, R_OK)) {
				instead_done();
				goto restart;
			} else {
				need_load = 0;
				printf("No file\n");
				free(opt_autoload); opt_autoload = NULL;
			}
		}
	}
	instead_cmd("save autosave", NULL);
	instead_done();
	exit(0);
}
