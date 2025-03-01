#undef _GNU_SOURCE
#define _GNU_SOURCE
#undef _BSD_SOURCE
#define _BSD_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <libdragon.h>

#include "../gnuboy/sys.h"
#include "../gnuboy/defs.h"
#include "../gnuboy/rc.h"

static void my_usleep(unsigned int us)
{
    uint32_t start = TICKS_READ();
    while( TICKS_SINCE(start) <= TICKS_FROM_US(us)) {; }
}

uint32_t last_timer = 0;

void *sys_timer()
{
    return &last_timer;
}

int sys_elapsed(void* prev)
{
    uint32_t now = TICKS_READ();
    uint32_t elapsed_us = TICKS_TO_US(TICKS_DISTANCE(*(uint32_t*)prev, now)); 
	*(uint32_t*)prev = now;
    return (int) elapsed_us;
}

void sys_sleep(int us)
{
    return; // emu is slow anyway, let's not sleep for now
	if (us <= 0) return;
	my_usleep(us);
}

void sys_checkdir(char *path, int wr)
{
    (void) path;
    (void) wr;
}

void sys_initpath(char *exe)
{
	char *buf, *home, *p;

	home = strdup(exe);
	sys_sanitize(home);
	p = strrchr(home, '/');
	if (p) *p = 0;
	else
	{
		buf = ".";
		rc_setvar("rcpath", 1, &buf);
		rc_setvar("savedir", 1, &buf);
		return;
	}
	buf = malloc(strlen(home) + 8);
	sprintf(buf, ".;%s/", home);
	rc_setvar("rcpath", 1, &buf);
	sprintf(buf, ".");
	rc_setvar("savedir", 1, &buf);
	free(buf);
}

void sys_sanitize(char *s)
{
}



