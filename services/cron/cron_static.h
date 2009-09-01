/*
 *
 * (c) by Alexander Neumann <alexander@bumpern.de>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License (either version 2 or
 * version 3) as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * For more information on the GPL, please go to:
 * http://www.gnu.org/copyleft/gpl.html
 */

#ifndef _CRON_STATIC_H
#define _CRON_STATIC_H

/* structures */

/* meaning of the signed values in cron_event_t (eg for minute):
 *   x in 0..59:    absolute value (minute)
 *   x is -1:       * (wildcard)
 *   x in -59..-2:  * /(-x) (step value (-x), 1 < -x < 60)
 */
typedef void (* cron_static_handler_t) (void);
struct cron_static_event_t {
    union{
        int8_t fields[5];
        struct {
            int8_t minute;
            int8_t hour;
            int8_t day;
            int8_t month;
            int8_t dow;
        };
    };
    cron_static_handler_t handler;
    uint8_t use_utc;
};


/* constants and global variables */


/* prototypes */

/** periodically check, if an event matches the current time. must be called
  * once per minute */
void cron_static_periodic(void);

#endif /* _CRON_STATIC_H */
