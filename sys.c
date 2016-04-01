/*
 * sys.c - Syscalls implementation
 */
#include <devices.h>

#include <utils.h>

#include <io.h>

#include <mm.h>

#include <mm_address.h>

#include <sched.h>

#include <errno.h>
#include <system.h>

#define LECTURA 0
#define ESCRIPTURA 1

int pid = 2;

int check_fd(int fd, int permissions)
{
  if (fd!=1) return -9; /*EBADF*/
  if (permissions!=ESCRIPTURA) return -13; /*EACCES*/
  return 0;
}

int sys_ni_syscall()
{
	return -38; /*ENOSYS*/
}

int sys_getpid()
{
	return current()->PID;
}

int sys_fork()
{
  int PID;
  
  if (list_empty(&freequeue)) PID = -1; // error
  struct list_head *firstFree = list_first(&freequeue);
  list_del(firstFree);
  struct task_struct *fork = list_head_to_task_struct(firstFree);
  
  ++pid;
  PID = pid;
  
  return PID;
}

void sys_exit()
{  
}

int sys_write(int fd, char * buffer, int size) {
  char auxBuffer[4];
	int res = 0;
  res = check_fd(fd,ESCRIPTURA);
	if (res != 0) return res;
	if (buffer == NULL) return -EFAULT;
	if (size < 0) return -EINVAL;
	while (size > 4) {
		copy_from_user(buffer,auxBuffer,4);
		res += sys_write_console(auxBuffer,4);
		buffer += 4;
		size -= 4;
	}
	// Acabamos de copiar los últimos elementos del buffer
	copy_from_user(buffer,auxBuffer,size);
	res += sys_write_console(auxBuffer,size);
	return res;
}

int sys_gettime() {
	return zeos_ticks;
}
