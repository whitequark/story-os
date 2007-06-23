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

class CallPacker
{
private:
char* packed;
unsigned int length;
unsigned int pointer;
unsigned int apointer;
char* name;
char* args;

public:
void dump();
CallPacker(char* name, char* args);
bool push(char b);
bool push(short w);
bool push(int d);
bool push(char* s);
void* data();
unsigned int size();
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
bool add(char* name, char* parameters, char* returnvalue);
bool present(char* name);
void require(char* name);
void wait(char* name);
bool present(char* name, char* parameters);
void require(char* name, char* parameters);
void wait(char* name, char* parameters);
};

#endif