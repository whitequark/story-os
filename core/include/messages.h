#ifndef _MESSAGES_H_
#define _MESSAGES_H_

#include <msgtype.h>

typedef struct SMessage
{
unsigned int sender;
MessageType type;
void* data;
unsigned int length;
SMessage *next, *prev;
} Message;

class Messenger
{
public:
Messenger();
};

#endif