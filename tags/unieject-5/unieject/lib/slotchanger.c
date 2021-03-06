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

#include <cdio/device.h>

#include <stdio.h>

int libunieject_slotchange(struct unieject_opts opts)
{
	if ( opts.caps )
	{
		// TODO: tell libcdio author about this
		cdio_drive_misc_cap_t unused, misc_cap;
		cdio_get_drive_cap((CdIo_t*)opts.cdio, &unused, &unused, &misc_cap);
		
#ifdef __FreeBSD__
		if ( strncmp("/dev/cd", opts.device, 7) != 0 )
			misc_cap = 0xFFFFFFFF;
#endif
	
		if ( ! (misc_cap & CDIO_DRIVE_CAP_MISC_SELECT_DISC) )
		{
			unieject_error(opts, _("the selected device doesn't have capability to select disc.\n"));
			return -2;
		}
	}

#if 0
	unieject_verbose(stdout, "%s: setting disc slot to %d\n", opts.progname, opts.slot);
	driver_return_code_t sts = cdio_set_disc((CdIo_t*)opts.cdio, opts.slot);
	
	return unieject_status(opts, sts);
#else
	unieject_error(opts, "libcdio doesn't support slot changing yet.\n");
	return -4;
#endif
}
