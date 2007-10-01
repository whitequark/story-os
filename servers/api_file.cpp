#include <file.h>
#include <story.h>
#include <string.h>

unsigned int File::root_fs_server;

File::File(char* name)
{
this->name = name;
server_tid = 0;
file_id = 0;
while(root_fs_server == 0)
 root_fs_server = get_root_fs();
}

int File::get_root_fs()
{
Message msg = {0};
msg.type = pcGetRootFS;
msg.receiver = PROCMAN_TID;
send(msg);
return msg.value1;
}

int File::create()
{
Message msg = {0};
msg.type = foCreate;
msg.data = name;
msg.data_length = strlen(name) + 1;
msg.receiver = root_fs_server;
send(msg);
server_tid = msg.sender;
file_id = msg.value1;
return msg.type;
}

int File::resolve()
{
Message msg = {0};
msg.type = foResolve;
msg.data = name;
msg.data_length = strlen(name) + 1;
msg.receiver = root_fs_server;
send(msg);
server_tid = msg.sender;
file_id = msg.value1;
mounted = msg.value2;
return msg.type;
}

int File::mount(unsigned int tid, unsigned int parameter)
{
Message msg = {0};
msg.type = foMount;
msg.data = name;
msg.data_length = strlen(name) + 1;
msg.receiver = root_fs_server;
msg.value1 = tid;
msg.value2 = parameter;
send(msg);
return msg.type;
}

int File::write(void* data, unsigned int length)
{
if(server_tid == 0)
 {
 int r = resolve();
 if(r != frOk)
  return r;
 }
Message msg = {0};
msg.type = foWrite;
msg.data = data;
msg.data_length = length;
msg.receiver = server_tid;
msg.value1 = file_id;
send(msg);
return msg.type;
}

int File::read(void* data, unsigned int length)
{
if(server_tid == 0)
 {
 int r = resolve();
 if(r != frOk)
  return r;
 }
Message msg = {0};
msg.type = foRead;
msg.reply = data;
msg.reply_length = length;
msg.receiver = server_tid;
msg.value1 = file_id;
send(msg);
return msg.type;
}

bool File::is_mounted()
{
return mounted;
}