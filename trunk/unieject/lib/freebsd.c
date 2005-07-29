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

   Portions of this files are under the following copyrights:
     Copyright (c) 1995-2000 Shunsuke Akiyama <akiyama@FreeBSD.org>.

   */

#include <unieject_internal.h>

#include <sys/param.h>
#include <sys/mount.h>
#include <stdio.h>
#include <errno.h>

char *checkmount(struct unieject_opts opts, char **device)
{
	char *ret = NULL;
	struct statfs *mntbuf;
	int mnts = getmntinfo(&mntbuf, MNT_NOWAIT);
	if (mnts == 0)
	{
		unieject_error(stderr, _("%s: error receiving mount information: %s\n"), opts.progname, strerror(errno));
		return *device;
	}
	
	int n;
	for (n = 0; n < mnts; n++)
	{
		// ignore special devices
		if ( mntbuf[n].f_mntfromname[0] != '/' ) continue;
		
		char *newdev = simplifylink(opts.progname, mntbuf[n].f_mntfromname);
		char *newmnt = simplifylink(opts.progname, mntbuf[n].f_mntonname);
		
		if ( strcmp(newdev, *device) == 0 )
		{
			unieject_verbose(stdout, _("%s: '%s' is mounted as '%s'\n"), opts.progname, *device, newmnt);
			ret = (newmnt == mntbuf[n].f_mntonname) ? sstrdup(mntbuf[n].f_mntonname) : newmnt;
			
			if ( newdev != mntbuf[n].f_mntfromname ) free(newdev);
			break;
		}

		if ( strcmp(newmnt, *device) == 0 )
		{
			unieject_verbose(stdout, _("%s: '%s' is the mount point of '%s'\n"), opts.progname, *device, newdev);
			ret = *device;
			*device = (newdev == mntbuf[n].f_mntfromname) ? sstrdup(mntbuf[n].f_mntfromname) : newdev;
			
			if ( newmnt != mntbuf[n].f_mntonname ) free(newmnt);
			break;
		}
		
		if ( newdev != mntbuf[n].f_mntfromname ) free(newdev);
		if ( newmnt != mntbuf[n].f_mntonname ) free(newmnt);
	}
	
	return ret;
}

bool internal_umountdev(struct unieject_opts opts, const char *device)
{
	struct unieject_opts nonverbose_opts = opts;
	nonverbose_opts.verbose = 0;
	
	char *mnt = NULL;
	
	while ( ( mnt = checkmount(opts, &device) ) )
	{
		if ( unmount(mnt, opts.force ? MNT_FORCE : 0) == -1 )
		{
			unieject_error(stderr, _("%s: unable to unmount '%s' [%s]\n"), opts.progname, mnt, strerror(errno));
			return false;
		}
		
		unieject_verbose(stdout, _("%s: '%s' unmounted from '%s'\n"), opts.progname, device, mnt);
	}

	return true;
}
