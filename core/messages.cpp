#include <messages.h>
#include <hal.h>
#include <list.h>
#include <string.h>

unsigned int syscall_message_send(Registers r)
{
Message *msg = ((Message*)r.ebx);
if(msg == NULL)
 return MSG_ERROR;
Task* dest = hal->taskman->task(msg->receiver);
Task* curr = hal->taskman->current;
if(dest == NULL || dest->wait_reason == wrDead)
 {
 if(dest == NULL)
  printf("send: no destination\n");
 else
  printf("send: destination dead\n");
 return MSG_ERROR;
 }
if((msg->data == NULL && msg->data_length != 0) ||
   (msg->data != NULL && msg->data_length == 0) ||
   (msg->reply == NULL && msg->reply_length != 0) ||
   (msg->reply != NULL && msg->reply_length == 0))
 {
 printf("send: malformed data/reply\n");
 return MSG_ERROR;
 }
msg->sender = curr->index;
if(msg->sender == msg->receiver)
 {
 printf("send: cannot send to self\n");
 return MSG_ERROR;
 }
//passed checks
Message* cmsg = new Message();
cmsg->value1 = msg->value1;
cmsg->value2 = msg->value2;
cmsg->value3 = msg->value3;
cmsg->type = msg->type;
cmsg->data_length = msg->data_length;
cmsg->reply_length = msg->reply_length;
cmsg->receiver = msg->receiver;
cmsg->sender = msg->sender;
if(msg->data_length != 0)
 {
 cmsg->data = new char[msg->data_length];
 memcpy(cmsg->data, msg->data, msg->data_length);
 }
if(msg->reply_length != 0)
 cmsg->reply = new char[msg->reply_length];
if(dest->messages == NULL)
 dest->messages = new List<Message*>(cmsg);
else
 dest->messages->add_tail(new List<Message*>(cmsg));
if(dest->wait_reason == wrMessage)
 {
 dest->wait_reason = wrNone;
 dest->resume_reason = rrOk;
 }
hal->taskman->current->wait_reason = wrReply;
hal->taskman->schedule();

//got reply
Message* reply = curr->reply;
msg->type = reply->type;
msg->value1 = reply->value1;
msg->value2 = reply->value2;
msg->value3 = reply->value3;
msg->data_received = reply->data_received;
msg->reply_sent = reply->reply_sent;
msg->sender = reply->sender;
if(msg->reply_length != 0)
 memcpy(msg->reply, reply->reply, msg->reply_length);

delete reply;

return MSG_OK;
}

unsigned int syscall_message_receive(Registers r)
{
Task* curr = hal->taskman->current;
if(curr->messages == NULL)
 {
 curr->wait_reason = wrMessage;
 hal->taskman->schedule();
 curr = hal->taskman->current;
 if(curr->resume_reason != rrOk)
  return MSG_INTERRUPTED;
 }
//got message at this point

Message *msg = curr->messages->item, *umsg = (Message*) r.ebx;
if(umsg == NULL)
 return MSG_ERROR;
if((umsg->data == NULL && umsg->data_length != 0) ||
   (umsg->data != NULL && umsg->data_length == 0))
 return MSG_ERROR;

umsg->type = msg->type;
umsg->value1 = msg->value1;
umsg->value2 = msg->value2;
umsg->value3 = msg->value3;
umsg->sender = msg->sender;
if(umsg->data_length != 0 && msg->data_length != 0)
 memcpy(umsg->data, msg->data, umsg->data_length > msg->data_length ? msg->data_length : umsg->data_length);
umsg->data_received = msg->data_length;
msg->data_received = umsg->data_length;
umsg->reply_length = msg->reply_length;

return MSG_OK;
}

unsigned int syscall_message_reply(Registers r)
{
Task* curr = hal->taskman->current;
Message *msg = curr->messages->item, *umsg = (Message*) r.ebx;
Task* dest = hal->taskman->task(msg->sender);
if(umsg == NULL)
 return MSG_ERROR;
if((umsg->reply == NULL && umsg->reply_length != 0) ||
   (umsg->reply != NULL && umsg->reply_length == 0))
 return MSG_ERROR;
/*if(msg->sender != 0)
 {*/
 msg->type = umsg->type;
 msg->value1 = umsg->value1;
 msg->value2 = umsg->value2;
 msg->value3 = umsg->value3;
 msg->sender = curr->index;
 msg->reply_sent = umsg->reply_length;
 if(umsg->reply_length != 0 && msg->reply_length != 0)
  memcpy(msg->reply, umsg->reply, msg->reply_length > umsg->reply_length ? msg->reply_length : umsg->reply_length);
 dest->reply = msg;
 if(dest->wait_reason == wrReply)
  dest->wait_reason = wrNone;
 /*}*/

//advance
List<Message*>* next = curr->messages->next;
delete curr->messages;
curr->messages = next;

return MSG_OK;
}

unsigned int syscall_message_forward(Registers r)
{
Task* curr = hal->taskman->current;
Message *umsg = (Message*) r.ebx;
List<Message*>* cmsg = curr->messages;
if(umsg == NULL)
 return MSG_ERROR;
Task* dest = hal->taskman->task(umsg->receiver);
if(dest == NULL)
 return MSG_ERROR;
if((umsg->data == NULL && umsg->data_length != 0) ||
   (umsg->data != NULL && umsg->data_length == 0))
 return MSG_ERROR;

cmsg->item->receiver = umsg->receiver;
cmsg->item->type = umsg->type;
cmsg->item->value1 = umsg->value1;
cmsg->item->value2 = umsg->value2;
cmsg->item->value3 = umsg->value3;
cmsg->item->data_length = umsg->data_length;
delete[] cmsg->item->data;
if(cmsg->item->data_length != 0)
 {
 cmsg->item->data = new char[cmsg->item->data_length];
 memcpy(cmsg->item->data, umsg->data, cmsg->item->data_length);
 }

List<Message*>* next = cmsg->next;
cmsg->remove();
curr->messages = next;

if(dest->messages != NULL)
 dest->messages->add_tail(cmsg);
else
 dest->messages = cmsg;
if(dest->wait_reason == wrMessage)
 {
 dest->wait_reason = wrNone;
 dest->resume_reason = rrOk;
 }

return MSG_OK;
}

Messenger::Messenger()
{
hal->syscalls->add(SYSCALL_SEND, &syscall_message_send);
hal->syscalls->add(SYSCALL_RECEIVE, &syscall_message_receive);
hal->syscalls->add(SYSCALL_REPLY, &syscall_message_reply);
hal->syscalls->add(SYSCALL_FORWARD, &syscall_message_forward);
}

int send(Message& msg)
{
int ret;
RSYSCALL1(SYSCALL_SEND, ret, &msg);
return ret;
}

int receive(Message& msg)
{
int ret;
RSYSCALL1(SYSCALL_RECEIVE, ret, &msg);
return ret;
}

int reply(Message& msg)
{
int ret;
RSYSCALL1(SYSCALL_REPLY, ret, &msg);
return ret;
}

int forward(Message& msg)
{
int ret;
RSYSCALL1(SYSCALL_FORWARD, ret, &msg);
return ret;
}