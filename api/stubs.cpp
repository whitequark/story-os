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

#include <stdlib.h>

void* operator new(unsigned int size)
{
return calloc(size, 1);
}

void* operator new[](unsigned int size)
{
return calloc(size, 1);
}

void operator delete(void* p)
{
free(p);
}

void operator delete[](void* p)
{
free(p);
}

void* operator new(size_t, void* p)
{
return p;
}

void* operator new[](size_t, void* p)
{
return p;
}
