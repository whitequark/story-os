#ifndef _SERVICE_H_
#define _SERVICE_H_

#include <task.h>

typedef struct SService
{
unsigned int index;
unsigned int start;
unsigned int length;
char* cmdline;

bool norestart;

unsigned int restart_counter;

unsigned int r_start;
Task* r_task;

SService* next;
} Service;

class ServiceManager
{
private:
Service* service;
unsigned int index;
Service* find(unsigned int index);
Service* find(Task* task);

public:
ServiceManager();
unsigned int load(unsigned int start, unsigned int length, char* cmdline, unsigned int restart_counter = 3);
Task* start(unsigned int index);
void stop(unsigned int index);
void process_kill(Task* task);
};

#endif