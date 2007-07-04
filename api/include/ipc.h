#ifndef _IPC_H_
#define _IPC_H_

#include <msgtype.h>

#define PROCMAN_TID 2

struct Message
{
unsigned int size;
void* buffer;
unsigned int type;
unsigned int task;
};

class Messenger
{
public:
bool send(Message msg);
bool receive(Message& msg);
bool reply(Message msg);
};

class Interface
{
private:
Interface();
char* name;

public:
Interface(char* name);
bool add();
bool present();
void require();
void wait();
unsigned int task();
};

#endif