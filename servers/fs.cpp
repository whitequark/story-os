#include <story.h>
#include <list.h>
#include <string.h>
#include <file.h>

struct Node
{
List<Node*>* children;
char* name;
unsigned int id;
unsigned int mounted_tid;
unsigned int mounted_parameter;
};

List<Node*>* nodes;
unsigned int current_id = 0;

List<char*>* strip_path(char* _path)
{
int n = 0;
if(_path[0] != '/')
 return NULL;
char* path = strdup(_path);
List<char*>* stripped = NULL, *p;
for(int i = 0; i < strlen(_path)+1; i++)
 if(path[i] == '/' || (path[i] == 0 && path[i-1] != 0))
  {
  path[i] = 0;
  if(stripped == NULL)
   stripped = new List<char*>(&path[n]);
  else 
   stripped->add_tail(new List<char*>(&path[n]));
  n = i+1;
  }
return stripped;
}

List<Node*>* resolve(char* path, char* mount_path = NULL)
{
List<char*>* stripped = strip_path((char*)path), *i;
List<Node*> *n = nodes, *ni, *parent;
iterate_list(i, stripped)
 {
 List<Node*>* found = NULL;
 iterate_list(ni, n)
  {
  if(n == NULL)
   return NULL;
  if(!strcmp(i->item, ni->item->name))
   {
   found = ni;
   n = ni->item->children;
   parent = ni;
   break;
   }
  }
 if(found == NULL)
  {
  if(parent == NULL)
   return NULL;
  if(parent->item->mounted_tid == 0)
   return NULL;
  else
   {
   if(mount_path != NULL)
    {
    List<char*>* first = i;
    for(; i; i = i->next)
     {
     if(i != first)
      strcat(mount_path, "/");
     strcat(mount_path, i->item);
     }
    }
   return parent;
   }
  }
 if(found && i->next == NULL)
  return found;
 }
}

void show(List<Node*>* n, int indent = 0)
{
List<Node*>* i;
if(n == NULL)
 return;
iterate_list(i, n)
 {
 for(int k = 0; k < indent; k++)
  printf(" ");
 printf("/%s (%d)\n", i->item->name, i->item->id);
 show(i->item->children, indent + 1);
 }
}

int main()
{
Message m = {0};
m.receiver = PROCMAN_TID;
m.type = pcSetRootFS;
send(m);

nodes = new List<Node*>(new Node);
nodes->item->name = "";
nodes->item->id = current_id++;

while(1)
 {
 Message msg = {0};
 char data[MAX_PATH]; 
 msg.data = data;
 msg.data_length = MAX_PATH;
 receive(msg);
 
 if(msg.data_received > msg.data_length)
  {
  msg.type = frPathTooLong;
  reply(msg);
  continue;
  }
 
 List<char*> *stripped, *i;
 List<Node*> *n = nodes, *ni, *parent = NULL, *found;
 unsigned int id;
 char mount_path[MAX_PATH] = {0};
 switch(msg.type)
  {
  case foCreate:
  stripped = strip_path((char*) msg.data);
  iterate_list(i, stripped)
   {
   if(n == NULL)
    {
    Node* nn = new Node;
    nn->id = current_id++;
    nn->name = strdup(i->item);
    parent->item->children = new List<Node*>(nn);
    n = parent->item->children;
    parent = n;
    n = n->item->children;
    }
   else
    {
    if(n != NULL && n->item->id == 0) // root
     {
     parent = n;
     n = parent->item->children;
     continue;
     }
    List<Node*>* found = NULL;
    iterate_list(ni, n)
     if(!strcmp(ni->item->name, i->item))
      {
      found = ni;
      break;
      }
    if(found)
     {
     n = found->item->children;
     parent = found;
     }
    else
     {
     Node* nn = new Node;
     nn->id = current_id++;
     nn->name = strdup(i->item);
     List<Node*>* nl = new List<Node*>(nn);
     parent->item->children->add_tail(nl);
     parent = nl;
     n = nl->item->children;
     id = nn->id;
     }
    }
   }
  msg.type = frOk;
  msg.value1 = resolve((char*)msg.data)->item->id;
  break;
  
  case foResolve:
  found = resolve((char*) msg.data, mount_path);
  if(found)
   {
   if(found->item->mounted_tid != 0)
    {
    msg.receiver = found->item->mounted_tid;
    msg.value2 = found->item->mounted_parameter;
    strcpy((char*) msg.data, mount_path);
    forward(msg);
    continue;
    }
   else
    {
    msg.value1 = found->item->id;
    msg.value2 = 0;
    msg.type = frOk;
    }
   }
  else
   msg.type = frFileNotFound;
  break;
  
  case foMount:
  found = resolve((char*) msg.data);
  if(found)
   if(found->item->mounted_tid == 0)
    {
    found->item->mounted_tid = msg.value1;
    found->item->mounted_parameter = msg.value2;
    msg.type = frOk;
    }
   else
    msg.type = frAlreadyMounted;
  else
   msg.type = frFileNotFound;
  break;
  
  default:
  msg.type = frCommandNotSupported;
  }
 reply(msg);
 }
}