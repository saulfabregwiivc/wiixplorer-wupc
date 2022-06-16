#include <stdarg.h>
#include "Controls/Application.h"
#include "Controls/GXConsole.hpp"

static GXConsole * Console = NULL;

extern "C" void gxprintf(const char * format, ...)
{
	if(!Console || Application::isClosing())
		return;

	char *tmp=0;
	va_list va;
	va_start(va, format);
	if((vasprintf(&tmp, format, va)>=0) && tmp)
	{
		Console->printf(tmp);
	}
	va_end(va);

	if(tmp)
		free(tmp);
}

extern "C" void SetGXConsole(void * console)
{
	Console = (GXConsole *) console;
}
