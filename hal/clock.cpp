//    This file is part of the Story OS
//    Copyright (C) 2007  Catherine 'whitequark'
//
//    Story OS is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    Story OS is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA. 

#include <clock.h>
#include <hal.h>

Clock::Clock()
{
ticks = 0;
hz = 1000;
hal->outb(0x43, 0x36);
hal->outb(0x40, (1193180 / (unsigned int)hz) & 0xff);
hal->outb(0x40, ((1193180 / (unsigned int)hz) >> 8) & 0xff); 
}

void Clock::tick()
{
ticks++;
}

unsigned int Clock::ms_to_ticks(unsigned int ms)
{
return ms * hz / 1000;
}
