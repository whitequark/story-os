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

#include <story.h>
#include <terminal.h>
#include <ipc.h>
#include <assert.h>
#include <string.h>
#include <filesystem.h>

int main()
{
Terminal t;
assert(Interface("fs").add() == 0);
MessageQuery q;
while(1)
 {
 q.wait();
 if(q.type() == File::mtResolve)
  {
  char path[q.length() + 1];
  q.data(path);
  
  FileID id;
  id.device_service = 0;
  id.device_id = 0;
  id.filesystem_service = 0;
  id.filesystem_id = 0;
  
  t.put_string(path);
  if(!strcmp(path, "/"))
   {
   id.filesystem_service = Interface("fs").task();
   id.filesystem_id = 1;
   }
  Message(&id, sizeof(id)).reply();
  }
 else
  Message(NULL, 0).reply();
 }
}