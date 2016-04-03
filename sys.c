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

int sys_fork(void)
{
  int PID = -1;
  
  if (list_empty(&freequeue)) return -1; // error
  struct list_head *firstFree = list_first(&freequeue);
  list_del(firstFree);

  union task_union * childUnion = (union task_union*)list_head_to_task_struct(firstFree);
  copy_data(current(),&childUnion->task,sizeof(struct task_struct);
 
  allocate_DIR(&childUnion->task);

  int pag, new_ph_pag, i;
  page_table_entry *child_PT = get_PT(&childUnion->task);
	for (pag = 0;pag<NUM_PAG_DATA;++pag) {
		new_ph_pag = alloc_frame();
		if (new_ph_pag != -1) {
			set_ss_pag(child_PT,PAG_LOG_INIT_DATA+pag,new_ph_pag);
		} else {
			for (i = 0; i < pag; ++i) {
				unsigned int frameToFree= get_frame(child_PT,PAG_LOG_INIT_DATA+pag);
				free_frame(frameToFree);
				del_ss_pag(child_PT,PAG_LOG_INIT_DATA+pag);
			}
			list_add_tail(firstFree,&freequeue);

			return -1; //error
		}
	} 

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
