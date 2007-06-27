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

#include <system.h>
#include <string.h>
#include <ipc.h>
#include <assert.h>
#include <stdio.h>

int main()
{
/*Interface("keyboard").wait("get");
char c = Interface("keyboard").call("get", "");
printf("got %c\n", c);*/
/*printf("app: send\n");
Message(mtUnknown, 3, (void*) "test", 5).send();
printf("app: sent\n");
printf("app: %s reply\n", Reply().check() ? "have" : "have not");
char reply[Reply().length()];
Reply().data(reply);
printf("app: got reply '%s'\n", reply);
Reply().remove();
printf("app: removed reply\n");
printf("app: %s reply\n", Reply().check() ? "have" : "have not");*/
CallPacker cp("getk", "");
cp.dump();

printf("app: sending...\n");
Message(mtFunction, 2, cp.data(), cp.size()).send();

printf("app: got reply\n");

char reply[Reply().length()];
Reply().data(reply);
printf("app: reply: %c\n", reply[0]);
}
