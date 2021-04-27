#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#ifdef _WIN32
#include <windows.h>
#endif
#include "instead/src/instead/instead.h"

#define WIDTH 70

static int opt_log = 0;
static int opt_debug = 0;
static int opt_width = WIDTH;

static int tiny_init(void)
{
	int rc;
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
	while ((*eptr == '\n' || *eptr == 0) && eptr != str) *eptr-- = 0;
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

int main(int argc, const char **argv)
{
	int rc, i;
	char *str;
	const char *game = NULL;
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

	if (instead_init(game)) {
		fprintf(stdout, "Can not init game: %s\n", game);
		exit(1);
	}
	if (instead_load(NULL)) {
		fprintf(stdout, "Can not load game: %s\n", instead_err());
		exit(1);
	}
#if 0 /* no autoload */
	str = instead_cmd("load autosave", &rc);
#else
	str = instead_cmd("look", &rc);
#endif
	if (!rc) {
		trim(str);
		fmt(str, opt_width);
		fflush(stdout);
	}
	free(str);

	while (1) {
		char input[256], *p, cmd[256 + 64];
		printf("> "); fflush(stdout);
		p = fgets(input, sizeof(input), stdin);
		if (!p)
			break;
		p[strcspn(p, "\n\r")] = 0;
		if (!strcmp(p, "quit"))
			break;

		if (!strncmp(p, "load ", 5) || !strncmp(p, "save ", 5)) {
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
			snprintf(cmd, sizeof(cmd), "@metaparser \"%s\"", p);
			str = instead_cmd(cmd, NULL);
		}
		if (str) {
			trim(str);
			fmt(str, opt_width);
			fflush(stdout);
		}
		free(str);
		if (!rc) /* no parser */
			footer();
		if (opt_log)
			fprintf(stderr, "%s\n", p);
	}
	instead_cmd("save autosave", NULL);
	instead_done();
	exit(0);
}
