#ifndef _MESSAGES_H_
#define _MESSAGES_H_

#define SYSCALL_SEND 10
#define SYSCALL_RECEIVE 11
#define SYSCALL_REPLY 12
#define SYSCALL_FORWARD 13

struct Message
{
void* data;
unsigned int data_length;
unsigned int data_received;

void* reply;
unsigned int reply_length;
unsigned int reply_sent;

unsigned int type;
unsigned int value1;
unsigned int value2;
unsigned int value3;

unsigned int sender;
unsigned int receiver;
};

class Messenger
{
public:
Messenger();
};

#endif