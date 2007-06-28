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

#include <terminal.h>
#include <string.h>
#include <ipc.h>

Terminal::Terminal()
{
Interface("terminal").wait();
}

void Terminal::put_char(char c)
{
Message(Terminal::mtPutChar, Interface("terminal").task(), (void*) &c, 1).send();
}

void Terminal::color(char c)
{
Message(Terminal::mtColor, Interface("terminal").task(), (void*) &c, 1).send();
}

void Terminal::put_string(char* s)
{
Message(Terminal::mtPutString, Interface("terminal").task(), (void*) s, strlen(s) + 1).send();
}