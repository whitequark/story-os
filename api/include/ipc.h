#ifndef _IPC_H_
#define _IPC_H_

#include <msgtype.h>

class MessageQuery
{
public:
bool pending();
void clear();
void remove();
MessageType type();
unsigned int length();
void data(char* data);
char* alloc_data();
unsigned int sender();
void wait();
};

class Message
{
MessageType type;
unsigned int receiver, length;
void* data;

public:
Message(MessageType type, unsigned int receiver, void* data, unsigned int length);
bool send();
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
};

#endif