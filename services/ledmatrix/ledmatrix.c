/*
 * Copyright (C) 2008 Bjoern Biesenbach <bjoern@bjoern-b.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 */

/*
 * This is the most simple server to obtain data for the 64x16 display
 */
#include "protocols/uip/uip.h"
#include "ledmatrix.h"

#define LED_SELECT_PORT PORTA
#define LED_SELECT_DDRD DDRA

#define LED_SELECT_1 PA0
#define LED_SELECT_2 PA1
#define LED_SELECT_3 PA2
#define LED_SELECT_4 PA3

#define LED_CONTROL_PORT PORTC
#define LED_CONTROL_DDRD DDRC

#define LED_RESET PC0
#define LED_BRIGHT PC1
#define LED_CLOCK PC2
#define LED_GREEN PC3
#define LED_RED PC4
#define LED_BRTWRT PC5
#define LED_BRTCLK PC6

/* Arrays für Rot und Gruen
 * Ein uint16_t pro Zeile pro Modul
 */
static uint16_t MODULE_RED[2*4*16];
static uint16_t MODULE_GREEN[2*4*16];

static uint16_t *frontbuffer_red;
static uint16_t *backbuffer_red;
static uint16_t *frontbuffer_green;
static uint16_t *backbuffer_green;

static void 
swap_buffers(void)
{
	uint16_t *tmp;
	tmp = backbuffer_red;
	backbuffer_red = frontbuffer_red;
	frontbuffer_red = tmp;

	tmp = backbuffer_green;
	backbuffer_green = frontbuffer_green;
	frontbuffer_green = tmp;
}

static void
LMSCall()
{
	uint16_t counter;
	static uint16_t *led_ptr;
	uint8_t *pBuffer = (uint8_t *) uip_appdata;

	/* wird nur direkt nach connect ausgefuehrt */
	if(uip_connected())
	{
		uip_conn->appstate.ledmatrix.byte_counter = 0;
		led_ptr = backbuffer_red;
		uip_conn->appstate.ledmatrix.toggler = 0;
	}
	else if(uip_newdata() || uip_acked())
	{
		for(counter=0;counter < uip_len; counter++)
		{
			if(!uip_conn->appstate.ledmatrix.toggler)
			{
				*led_ptr = *pBuffer++;
				uip_conn->appstate.ledmatrix.toggler = 1;
			}
			else
			{
				*led_ptr |= (*pBuffer++<<8) & 0xFF00;
				led_ptr++;
				uip_conn->appstate.ledmatrix.toggler = 0;
			}
			if(++uip_conn->appstate.ledmatrix.byte_counter == 128)
				led_ptr = backbuffer_green;
			else if(uip_conn->appstate.ledmatrix.byte_counter == 256)
			{
				swap_buffers();
//				uip_close();
				led_ptr = backbuffer_red;
				uip_conn->appstate.ledmatrix.byte_counter = 0;
			}
		}
	}
}

void
ledmatrix_init()
{
	uip_listen(HTONS(PORT_LMS), LMSCall);
}


static void
led_update(void)
{
	uint8_t counter,counter2,i;
	
	/* reset */
	LED_CONTROL_PORT |= (1<<LED_RESET);
	LED_CONTROL_PORT &= ~(1<<LED_RESET);

	uint16_t *red_ptr = frontbuffer_red;
	uint16_t *green_ptr = frontbuffer_green;
	for(i=0;i<4;i++)
	{
		LED_SELECT_PORT |= (1<<i);
		for(counter2 = 0;counter2 < 16;counter2++)
		{
	//		LED_CONTROL_PORT |= (1<<LED_BRIGHT);
	//		LED_CONTROL_PORT |=  (1<<LED_BRIGHT);
			for(counter = 0; counter < 16; counter++)
			{
				if((*red_ptr>>counter)&1)
					LED_CONTROL_PORT |= (1<<LED_RED);
				else
					LED_CONTROL_PORT &= ~(1<<LED_RED);
				if((*green_ptr>>counter)&1)
					LED_CONTROL_PORT |= (1<<LED_GREEN);
				else
					LED_CONTROL_PORT &= ~(1<<LED_GREEN);
				LED_CONTROL_PORT |= (1<<LED_CLOCK);
				LED_CONTROL_PORT &= ~(1<<LED_CLOCK);
			}
			red_ptr++;
			green_ptr++;
	//		LED_CONTROL_PORT &= ~(1<<LED_BRIGHT);
		}
		LED_SELECT_PORT &= ~(1<<i);
	}
}

void led_init(void)
{
	/* Array initialisieren */
	memset(MODULE_RED,0,sizeof(MODULE_RED));
	memset(MODULE_GREEN,0,sizeof(MODULE_GREEN));

	frontbuffer_red = MODULE_RED;
	frontbuffer_green = MODULE_GREEN;
	backbuffer_red = MODULE_RED + (16 * 4);
	backbuffer_green = MODULE_GREEN + (16 * 4);

	LED_SELECT_DDRD |= (1<<LED_SELECT_1) | 
		(1<<LED_SELECT_2) | 
		(1<<LED_SELECT_3) | 
		(1<<LED_SELECT_4);

	LED_CONTROL_DDRD |= (1<<LED_RESET) | 
		(1<<LED_BRIGHT) |
		(1<<LED_CLOCK) |
		(1<<LED_GREEN) |
		(1<<LED_RED) |
		(1<<LED_BRTWRT) | 
		(1<<LED_BRTCLK);

	LED_SELECT_PORT = 0;
	LED_CONTROL_PORT = 0;
	LED_CONTROL_PORT |= 1<<LED_BRTWRT;
}

void led_runner(void)
{


	led_update();
	
	LED_CONTROL_PORT |= (1<<LED_CLOCK);
	LED_CONTROL_PORT &= ~(1<<LED_CLOCK);
	
}

/*
  -- Ethersex META --
  header(services/ledmatrix/ledmatrix.h)
  net_init(ledmatrix_init)
  init(led_init)
  mainloop(led_runner)
*/

