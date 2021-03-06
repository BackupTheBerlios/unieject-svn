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

#include <unieject.h>
#include <unieject_internal.h>

#include <cdio/mmc.h>

#include <stdio.h>

#ifdef __FreeBSD__
#	include <fcntl.h>
#	include <errno.h>
#	include <sys/ioctl.h>
#	include <sys/cdio.h>
#endif

int libunieject_eject(struct unieject_opts *opts)
{
	if ( opts->caps )
	{
		// TODO: tell libcdio author about this
		cdio_drive_misc_cap_t unused, misc_cap;
		cdio_get_drive_cap((CdIo_t*)opts->cdio, &unused, &unused, &misc_cap);

#ifdef __FreeBSD__
		if ( strncmp("/dev/cd", opts->device, 7) != 0 )
			misc_cap = 0xFFFFFFFF;
#endif

		if ( opts->eject )
		{
			if ( ! (misc_cap & CDIO_DRIVE_CAP_MISC_EJECT) )
			{
				unieject_error(*opts, _("the selected device doesn't have eject capabilities.\n"));
				return -2;
			}
		} else {
			if ( ! (misc_cap & CDIO_DRIVE_CAP_MISC_CLOSE_TRAY) )
			{
				unieject_error(*opts, _("the selected device doesn't have tray close capabilities.\n"));
				return -2;
			}
		}
	}
	
	if ( UNLIKELY(opts->fake) )
		return 0;

#ifdef __FreeBSD__
	if ( opts->eject )
	{
		int devfd = open(opts->device, O_RDONLY);
		if ( UNLIKELY(devfd == -1) )
		{
			unieject_error(*opts, _("unable to open device descriptor [%s].\n"), strerror(errno));
			return -4;
		}
		
		if ( UNLIKELY(ioctl(devfd, CDIOCALLOW) == -1) )
		{
			unieject_error(*opts, _("error in ioctl [%s].\n"), strerror(errno));
			return -5;
		}
		
		close(devfd);
	}
#endif
	
#ifdef __FreeBSD__
	driver_return_code_t sts;
	if ( strncmp("/dev/cd", opts->device, 7) != 0 )
	{
		if ( opts->eject )
			sts = cdio_eject_media((CdIo_t**)&opts->cdio);
		else
		{
			cdio_destroy(opts->cdio);
			opts->cdio = NULL;
			sts = cdio_close_tray(opts->device, NULL);
		}
	} else
		sts = mmc_start_stop_media((CdIo_t*)opts->cdio, opts->eject, 0, 0);
#elif defined(__APPLE__)
	driver_return_code_t sts;
	if ( opts->eject )
		sts = cdio_eject_media((CdIo_t**)&opts->cdio);
	else
	{
		cdio_destroy(opts->cdio);
		opts->cdio = NULL;
		sts = cdio_close_tray(opts->device, NULL);
	}
#else
	driver_return_code_t sts = mmc_start_stop_media((CdIo_t*)opts->cdio, opts->eject, 0, 0);
#endif
	
	return unieject_status(*opts, sts);
}
