/*
 * Copyright (C) 2009 Bjoern Biesenbach <bjoern@bjoern-b.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <avr/interrupt.h>
#include "protocols/uip/uip.h"

#ifdef STELLA_UDP_SIMPLE
#include "stella.h"

struct _rgbPacket
{
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	uint8_t smoothness;
};

void stella_simple_new_data(void)
{
	if(!uip_newdata()) return;
	struct _rgbPacket *pRgbPacket = uip_appdata; 
	stella_fade_step = pRgbPacket->smoothness;
	stella_setValue(1, 0, pRgbPacket->green);
	stella_setValue(1, 1, pRgbPacket->blue);
	stella_setValue(1, 3, pRgbPacket->red);
}
#endif
void
stella_simple_init(void)
{
#ifdef STELLA_UDP_SIMPLE
	uip_udp_conn_t *c;
	uip_ipaddr_t addr;

    uip_ipaddr(&addr, 0,0,0,0);
	c = uip_udp_new(&addr, 0, stella_simple_new_data);
	if(c != NULL)
	{
		uip_udp_bind(c, HTONS(4477));
	}
#endif
}

/*
  -- Ethersex META --
  header(services/stella/stella_udp_simple.h)
  init(stella_simple_init)
*/


