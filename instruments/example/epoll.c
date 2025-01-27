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

#define _GNU_SOURCE
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <string.h>
#include <termios.h>
#include <pthread.h>
#include <sys/epoll.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>

#include <jni.h>
#include <stdlib.h>
#include "../base/log.h"

#include "../base/hook.h"
#include "../base/base.h"
#include "../base/util.h"
#include "../base/pesudo_mm.h"

#undef log

#define log(...)                                                    \
	{                                                               \
		FILE *fp = fopen("/data/local/tmp/adbi_example.log", "a+"); \
		if (fp)                                                     \
		{                                                           \
			fprintf(fp, __VA_ARGS__);                               \
			fclose(fp);                                             \
		}                                                           \
	}

// this file is going to be compiled into a thumb mode binary

// void __attribute__ ((constructor)) my_init(void);

static struct hook_t eph;

// for demo code only
static int counter;

// arm version of hook
extern int my_epoll_wait_arm(int epfd, struct epoll_event *events, int maxevents, int timeout);

/*  
 *  log function to pass to the hooking library to implement central loggin
 *
 *  see: set_logfunction() in base.h
 */
static void my_log(const char *msg)
{
	LOGD("%s", msg);
	// log("%s", msg)
}

int my_epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout)
{
	int (*orig_epoll_wait)(int epfd, struct epoll_event *events, int maxevents, int timeout);
	orig_epoll_wait = (void *)eph.orig;

	hook_precall(&eph);
	int res = orig_epoll_wait(epfd, events, maxevents, timeout);
	if (counter)
	{
		hook_postcall(&eph);
		log("epoll_wait() called\n");
		counter--;
		if (!counter)
			log("removing hook for epoll_wait()\n");
	}

	return res;
}

int fake_connect(int s, const struct sockaddr * addr, socklen_t l)
{
    log("##### connect ####");
    return connect(s, addr, l);
}

void my_init(void)
{
	char content[512];
	struct vm_area_struct vma[10], *mm;
	int i, nvma = 10;

	counter = 3;

	set_logfunction(my_log);

	LOGD("%s started\n", __FILE__);

	get_module_map(getpid(), "/system/lib/libc.so", vma, &nvma);
	// LOGD("===============");
	// for(i = 0; i < nvma; i++) {
    //     serialize_vm_area_item(&vma[i], content);
    //     LOGD("%s", content);
    // }
	LOGD("===============");
	// for (i = 0; i < nvma; i++)
	// {
	// 	mm = &vma[i];
	// 	LOGD("<< mprotect %lx - %lx ", mm->vm_start, mm->vm_end);
	// 	LOGD("-<< mprotect %lx - %lx ", mm->vm_start, mm->vm_end);
	// 	if (mprotect((void *)mm->vm_start, mm->vm_end - mm->vm_start, PROT_READ | PROT_WRITE))
	// 	{
	// 		LOGD("<< mprotect %lx - %lx error: %d", mm->vm_start, mm->vm_end, errno);
	// 	}
	// 	else
	// 	{
	// 		LOGD("<< mprotect %lx - %lx ", mm->vm_start, mm->vm_end);
	// 	}
	// }

	hook(&eph, getpid(), "/system/lib/libc.so", "connect", my_epoll_wait_arm, fake_connect);

	// for (i = 0; i < nvma; i++)
	// {
	// 	mm = &vma[i];
	// 	if (mprotect((void *)mm->vm_start, mm->vm_end - mm->vm_start, mm->vm_flags))
	// 	{
	// 		LOGD("<< mprotect %lx - %lx error: %d", mm->vm_start, mm->vm_end, errno);
	// 	}
	// }
}

int Inject_entry()
{
	LOGD("Inject_entry Func is called\n");
	my_init();
	return (int)my_epoll_wait;
}
