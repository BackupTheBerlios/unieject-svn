/* unieject - Universal eject command
   Copyright (C) 2005, Diego Pettenò

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

   */

#include <unieject_internal.h>

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef HAVE_LIBGEN_H
#	include <libgen.h>
#endif

char *simplifylink(const char *orig)
{
#if defined(HAVE_READLINK) && defined(HAVE_DIRNAME)
	char *tmp = (char*)malloc(1024);
	
	int c = readlink(orig, tmp, 1023);
	if ( c != -1)
	{
		tmp[c] = '\0';
		if ( tmp[0] != '/' ) // relative link
		{
			char *copylink = sstrdup(orig);
			char *origdir = strdup(dirname(copylink));
			free(copylink);
			
			char *newname = NULL;
			asprintf(&newname, "%s/%s", origdir, tmp);
			free(tmp); free(origdir);
			
			tmp = newname;
		}
		
		return tmp;
	}
	
	free(tmp);
#endif
	return strdup(orig);
}

/**
 * @brief Output an error for unieject library
 * @param opts Options to apply for the output
 * @param format Output format of the error
 * @param ... As in printf() like function
 *
 * @note Internal function
 */
void unieject_error(const struct unieject_opts opts, const char *format, ...)
{
	if ( opts.verbose == -1 )
		return;
	
	char *newformat = NULL;
	if ( asprintf(&newformat, "%s: %s", opts.progname, format) == -1 )
		return;
	
	va_list argptr;
	va_start(argptr, format);
	
	vfprintf(stderr, newformat, argptr);
	
	va_end(argptr);
	
	free(newformat);
}

/**
 * @brief Output verbose message for unieject library
 * @param opts Options to apply for the output
 * @param format Output format of the message
 * @param ... As in printf() like function
 *
 * @note Internal function
 */
void unieject_verbose(const struct unieject_opts opts, const char *format, ...)
{
	if ( opts.verbose < 1 )
		return;
	
	char *newformat = NULL;
	if ( asprintf(&newformat, "%s: %s", opts.progname, format) == -1 )
		return;
	
	va_list argptr;
	va_start(argptr, format);
	
	vfprintf(stdout, newformat, argptr);
	
	va_end(argptr);
	
	free(newformat);
}

/**
 * @brief Outputs a message about the given status return code
 * @param opts Options to apply for the output
 * @param sts Status return code from libcdio library
 */
int unieject_status(const struct unieject_opts opts, int sts)
{
	switch(sts)
	{
	case DRIVER_OP_SUCCESS:
		return 0;
	default:
		unieject_error(opts, _("unable to execute command (unknown CDIO status: %d)\n"), sts);
	}
	
	return -3;
}
