#ifndef _PROCMAN_H_
#define _PROCMAN_H_

#include <ipc.h>

class Procman
{
private:
unsigned int procman_tid;
Messenger m;

public:
Procman();
Procman(bool); //don't wait for filesystem - only used in fs server

enum {	mtNone, mtDelay, mtDie, mtWaitForDie, mtAttachIRQ, mtDetachIRQ, mtAllocPages, mtCreateThread, mtWaitForMessage, mtSetFilesystemTID, mtWaitForFilesystem, mtGetFilesystemTID }; //TO procman
enum {	mtIRQFired }; //FROM procman
void delay(unsigned int millis);
void die(int return_code);
int wait_for_die(unsigned int task);
void attach_irq(unsigned int num);
void detach_irq(unsigned int num);
void wait_for_message();
void* alloc_pages(unsigned int count);
unsigned int create_thread(void* entry_point);
unsigned int get_tid();
unsigned int get_fs_server_tid();

void printf(char* fmt, ...);
};

#endif