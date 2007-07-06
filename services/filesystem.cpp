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
#include <story.h>
#include <ipc.h>
#include <string.h>
#include <procman.h>

Procman p;

FilesystemObject::FilesystemObject(char* name)
{
this->name = strdup(name);
this->children = NULL;
}

Filesystem::Filesystem()
{
files = new FSOList(new FilesystemObject(""));
}

void Filesystem::show()
{
show(0, files);
}

List<char*>* Filesystem::strip_path(char* _path)
{
List<char*>* stripped = NULL;
if(!strcmp(_path, "/"))
 return new List<char*>("");
char* path = strdup(_path);
int i, j = 0, len = strlen(path);
for(i = 0; i <= len; i++)
 if(path[i] == '/' || path[i] == 0)
  {
  if(stripped)
   stripped->add_tail(new List<char*>(&path[j]));
  else
   stripped = new List<char*>(&path[j]);
  path[i] = 0;
  j = i + 1;
  }
if(!strcmp(stripped->tail()->item, ""))
 delete stripped->tail();
return stripped;
}

FilesystemObject* Filesystem::get(char* nspath)
{
List<char*>* path = strip_path(nspath);
List<char*> *i;
FSOList* current = files;
FSOList* j;

iterate_list(i, path)
 {
 bool found = false;
 iterate_list(j, current)
  if(!strcmp(j->item->name, i->item))
   {
   found = true;
   break;
   }
 if(!found || (i->next && !j->item->children))
  return NULL;
 if(i->next)
  current = j->item->children;
 else
  current = j;
 }
return current->item;
}

void Filesystem::show(int level, FSOList* list)
{
FSOList* i;
iterate_list(i, list)
 {
 for(int k = 0; k < level; k++)
  p.printf(" ");
 p.printf("/%s\n", i->item->name);
 if(i->item->children)
  show(level+1, i->item->children);
 }
}

//don't ask me about HOW it works - it works, and that's good!
bool Filesystem::add(char* nspath)
{
List<char*>* path = strip_path(nspath);
List<char*> *i, *k, *s;
if(get(nspath))
 return false;

FSOList* current = files;
FSOList *j, *q;

iterate_list(i, path)
 {
 bool found = false;
 iterate_list(j, current)
  {
  if(!strcmp(j->item->name, i->item))
   {
   found = true;
   break;
   }
  }
 if(!found || (i->next && !j->item->children))
  break;
 if(i->next)
  {
  current = j->item->children;
  q = j;
  s = i;
  }
 else
  current = j;
 }

if(j == NULL) //if in last iteration none found
 {
 j = q;
 i = s;
 }

k = i->next;
current = j;
iterate_list(i, k)
 {
 FilesystemObject* obj = new FilesystemObject(i->item);
 FSOList* list = new FSOList(obj);
 if(!current->item->children)
  current->item->children = list;
 else
  current->item->children->add_after(list);
 current = list;
 }
return true;
}

int main()
{
Filesystem f;
f.add("/dev/hda");
f.add("/dev/hdb");
f.show();
FilesystemObject* obj = f.get("/dev/hda");
if(obj)
 p.printf("found: name '%s'\n", obj->name);
else
 p.printf("not found\n");
}