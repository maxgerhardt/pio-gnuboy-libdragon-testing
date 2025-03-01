#include "gnuboy/rc.h"

static char *romdir;

rcvar_t menu_exports[] =
{
	RCV_STRING("romdir", &romdir, "rom directory"),
	RCV_END
};
