#ifndef _MESSAGES_H_
#define _MESSAGES_H_

#include <msgtype.h>
#include <ipc.h>

struct CoreMessage
{
unsigned int sender;
unsigned int type;
void* data;
unsigned int length;
CoreMessage *next;
};

class CoreMessenger
{
public:
CoreMessenger();
bool send(unsigned int sender, Message* msg);
};

#endif