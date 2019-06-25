/*
 *  Collin's Binary Instrumentation Tool/Framework for Android
 *  Collin Mulliner <collin[at]mulliner.org>
 *  http://www.mulliner.org/android/
 *
 *  (c) 2012,2013
 *
 *  License: LGPL v2.1
 *
 */

#include <termios.h>

int find_name(pid_t pid, char *name, char *libn, unsigned long *addr);
int find_libbase(pid_t pid, char *libn, unsigned long *addr);

#define MAX_NAME_LEN 256
/* memory map for libraries */
struct mm {
	char name[MAX_NAME_LEN];
	unsigned long start, end;
};
int find_lib_map(pid_t pid, const char *libn, struct mm *pmm);


