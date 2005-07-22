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

#ifndef __UNIEJECT_H__
#define __UNIEJECT_H__

#include <cdio/cdio.h>

/**
 * @file
 * @brief Base interface to unieject functions
 */

/**
 * @brief Defines the options used to influence libunieject functions
 */
struct unieject_opts {
	int eject;		///< Will eject or close the tray?
	int fake;		///< Don't eject, just show device found.
	int verbose;		///< Enable verbose output.
	int unmount;		///< Unmount device if occupied.
	int speed;		///< Maximum speed to set the device to.
	
	char *device;		///< Path of the device to open.
};

/**
 * @brief Gets the default CDIO device name.
 * @param progname Name of the program to output for messages.
 * @param opts Options to apply.
 *
 * @note This should be internal only
 */
char *libunieject_defaultdevice(const char *progname, struct unieject_opts opts);

/**
 * @brief Gets the device name, resolving the argument or finding the default.
 * @param progname Name of the program to output for messages.
 * @param opts Options to apply.
 * @param basename Name of the device to canonicalize or NULL to find the default.
 */
char *libunieject_getdevice(const char *progname, struct unieject_opts opts, const char *basename);

/**
 * @brief Eject the media in the passed cdio descriptor.
 * @param progname Name of the program to output for messages.
 * @param opts Options to apply
 * @param cdio libcdio descriptor for opened device.
 *
 * @retval 0 Eject successful
 * @retval -2 Drive doesn't has the capabilities required
 * @retval -3 Error during ejection
 */
int libunieject_eject(const char *progname, struct unieject_opts opts, CdIo_t *cdio);

/**
 * @brief Unmount a device (platform dependant)
 * @param progname Name of the program to output for messages.
 * @param opts Options to apply.
 * @param device Normalized name of the device to unmount.
 *
 * @retval true Unmount completed successfully (or opts.fake enabled).
 * @retval false Error unmounting the device.
 */
bool libunieject_umountdev(char *progname, struct unieject_opts opts, const char *device);

#endif
