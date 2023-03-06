#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#include <story.h>
#include <messages.h>
#include <file.h>

#define SYSCALL0(n) asm("int $0x31"::"a"(n))
#define SYSCALL1(n, p1) asm("int $0x31"::"a"(n),"b"(p1))
#define SYSCALL2(n, p1, p2) asm("int $0x31"::"a"(n),"b"(p1),"c"(p2))
#define SYSCALL3(n, p1, p3) asm("int $0x31"::"a"(n),"b"(p1),"c"(p2),"d"(p3))
#define RSYSCALL0(n, ret) asm("int $0x31":"=a"(ret):"a"(n))
#define RSYSCALL1(n, ret, p1) asm("int $0x31":"=a"(ret):"a"(n),"b"(p1))
#define RSYSCALL2(n, ret, p1, p2) asm("int $0x31":"=a"(ret):"a"(n),"b"(p1),"c"(p2))
#define RSYSCALL3(n, ret, p1, p3) asm("int $0x31":"=a"(ret):"a"(n),"b"(p1),"c"(p2),"d"(p3))

#define MSG_OK 0
#define MSG_ERROR 1
#define MSG_INTERRUPTED 2

#define PROCMAN_TID 2

#define SYSCALL_GET_TID 1

typedef enum { pcMorecore = 1000, pcDie, pcGetRootFS, pcSetRootFS, pcStartThread, pcGainIOPrivilegies, pcAttachIRQ, pcDelay, pcAttachMemory, pcWaitDie } ProcmanCommands;
typedef enum { prOk, prNoPrivilegies, prFailed } ProcmanReplies;
typedef enum { pmIRQFired } ProcmanMessages;

/*
all buffers may be NULL, but only when _length is also 0

need for send:
 receiver, receiver != sender
need for receive:
 data, data_length
can read after receive:
 type, valueX, sender, data, data_length, data_received
need for reply:
 reply, reply_length
can read after reply:
 type, valueX, sender, reply, reply_length, data_received, reply_sent
need for forward:
 receiver
*/

int send(Message& msg);
int receive(Message& msg);
int reply(Message& msg);
int forward(Message& msg);

void die(int return_code);
unsigned int get_tid();
void printf(char* fmt, ...);
unsigned int start_thread(int(*address)());
void delay(unsigned int ms);
char getch();
unsigned int exec(char* path, char* parameters);
void wait_die(unsigned int tid);

extern File* stdin;
extern File* stdout;

#endif
