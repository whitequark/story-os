#ifndef _IPC_H_
#define _IPC_H_

#include <msgtype.h>

class Message
{
MessageType type;
unsigned int receiver, length;
void* data;

public:
Message(MessageType type, unsigned int receiver, void* data, unsigned int length);
bool send();
};

class MessageQuery
{
public:
bool pending();
};

#endif