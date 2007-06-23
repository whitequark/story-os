#ifndef _STORY_H_
#define _STORY_H_

#define NULL (0)
#define PAGE_SIZE 0x1000

#define __PACKED__ __attribute__((__packed__))

#include <version.h>

//#define _DEBUGGING_VMM_
//#define _DEBUGGING_MM_
//#define _DEBUGGING_PAGING_
//#define _DEBUGGING_TASKSWITCHER_
//#define _DEBUGGING_EXECUTABLE_LOADER_
//#define _DEBUGGING_ELF_LOADER_
//#define _DEBUGGING_INTERFACE_

//#define _ENABLE_KERNEL_SERIAL_
//#define _ENABLE_SERIAL_TERMINAL
//#define _ENABLE_SERIAL_GDB_STUB_

#ifdef _ENABLE_SERIAL_GDB_STUB_
#define _ENABLE_GDB_STUB_
#endif

#endif
