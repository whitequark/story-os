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

#include <file.h>
#include <procman.h>
#include <string.h>

File::File(char* name)
{
this->name = name;
Procman p;
fs_server = p.get_fs_server_tid();
p.printf("fs server tid = %i\n", fs_server);
}

bool File::open()
{
}

bool File::create()
{
Message msg;
msg.task = fs_server;
msg.type = File::mtCreate;
msg.size = strlen(name) + 1;
msg.buffer = name;
m.send(msg);

m.receive_reply(msg);
if(msg.type != rtOk)
 return false;
return true;
}