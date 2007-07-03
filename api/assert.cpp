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

#include <colors.h>
#include <system.h>
#include <terminal.h>

extern "C" void __assert_fail (__const char *__assertion, __const char *__file,
                           unsigned int __line, __const char *__function)
{
Terminal t;
t.color(LIGHTRED);
t.put_string("ASSERTION FAILED: ");
t.put_string(__assertion);
t.put_string("\nFile: ");
t.put_string(__file);
t.put_string(" Line: ");
t.put_string("FIXME!");
t.put_string(" Function: ");
t.put_string(__function);
t.put_string("\n");
t.color(LIGHTGRAY);
die(1);
}

