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

#include <filesystem.h>
#include <file.h>
#include <string.h>
#include <procman.h>

int main()
{
Procman p(true);
Filesystem fs;
Messenger m;

Message msg;
msg.task = p.get_tid();
msg.type = Procman::mtSetFilesystemTID;
msg.size = 0;
m.send(msg); //I am Filesystem Server

while(1)
 {
 p.wait_for_message();
 
 Message msg, reply;
 reply.size = 0;
 reply.buffer = NULL;
 reply.type = rtOk;
 msg.size = 0; //after receive() size = real message size
 msg.buffer = NULL;
 m.receive(msg);
 
 bool ret;
 
 switch(msg.type)
  {
  case File::mtCreate:
  msg.buffer = malloc(msg.size);
  m.receive(msg);
  ret = fs.add((char*) msg.buffer);
  if(!ret)
   reply.type = rtError;
  m.reply(reply);
  fs.show();
  break;
  
  default: 
  reply.type = rtError;
  m.reply(reply);
  }
 }
}