#ifndef _IPC_H_
#define _IPC_H_

#include <msgtype.h>

class MessageQuery
{
public:
bool pending();
MessageType type();
unsigned int length();
void data(void* data);
void* alloc_data();
unsigned int sender();
void wait();
bool next();
};

class Message
{
MessageType type;
unsigned int receiver, length;
void* data;

public:
Message(MessageType type, unsigned int receiver, void* data, unsigned int length);
Message(void* data, unsigned int length);
bool send();
bool reply();
};

class Reply
{
public:
bool check();
unsigned int length();
void data(void* data);
void remove();
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