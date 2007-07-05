//    This file is part of the Story OS
//    Copyright (C) 2007  Peter Zotov
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

#include <mutex.h>

Mutex::Mutex()
{
locker = 0;
}

bool Mutex::lock()
{
unsigned int previous;
do
 {
 previous = xchg(&locker, 1);
 } while(previous != 0);
}

bool Mutex::try_lock()
{
return xchg(&locker, 1) == 0;
}

bool Mutex::is_locked()
{
return locker;
}

bool Mutex::unlock()
{
return xchg(&locker, 0) == 0;
}