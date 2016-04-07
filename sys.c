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

int pid = 1000;

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

int ret_from_fork() {
	return 0;
}

int sys_fork(void)
{
  int PID = -1;
  
	/*Get a free task*/
  if (list_empty(&freequeue)) return -1; // error
  struct list_head *firstFree = list_first(&freequeue);
  list_del(firstFree);

	/*Copy the parent's task union to the child*/
  union task_union * childUnion = (union task_union*)list_head_to_task_struct(firstFree);
  copy_data(current(),&childUnion->task,sizeof(struct task_struct));
 
	/*Store process adress space*/
  allocate_DIR(&childUnion->task);

	/*Searching free physical pages*/
  int pag, new_ph_pag, i;
  page_table_entry *child_PT = get_PT(&childUnion->task);
	for (pag=0;pag<NUM_PAG_DATA;++pag) {
		new_ph_pag = alloc_frame(); // New page
		if (new_ph_pag != -1) {
			set_ss_pag(child_PT,PAG_LOG_INIT_DATA+pag,new_ph_pag);
		} else { // If there aren't enough pages, we should free allocated pages
			for (i = 0; i < pag; ++i) {
				unsigned int frameToFree= get_frame(child_PT,PAG_LOG_INIT_DATA+pag);
				free_frame(frameToFree);
				del_ss_pag(child_PT,PAG_LOG_INIT_DATA+pag);
			}
			list_add_tail(firstFree,&freequeue);

			return -1; //error
		}
	} 

	page_table_entry *parent_PT = get_PT(current());
	for (pag=0;pag<NUM_PAG_KERNEL;++pag) {
		set_ss_pag(child_PT,pag,get_frame(parent_PT,pag));
	} 
	for (pag=0;pag<NUM_PAG_CODE;++pag) {
		set_ss_pag(child_PT,PAG_LOG_INIT_DATA+pag,get_frame(parent_PT,PAG_LOG_INIT_DATA+pag));
	} 

	for (pag=NUM_PAG_KERNEL+NUM_PAG_CODE;pag<NUM_PAG_KERNEL+NUM_PAG_CODE+NUM_PAG_DATA;++pag) {
		set_ss_pag(parent_PT,pag+NUM_PAG_DATA,get_frame(child_PT,pag));
		copy_data((void*)(pag<<12),(void*)((pag+NUM_PAG_DATA)<<12),PAGE_SIZE);
		del_ss_pag(parent_PT,pag+NUM_PAG_DATA);
	}
  /*Deny access from parent to child*/
	set_cr3(get_DIR(current()));

	/*Assign PID*/
	childUnion->task.PID=++pid;
  PID = pid;

	int kernel_ebp;
	asm("movl %%ebp, %0;"
			: "=g" (kernel_ebp)
			:
		 );
  kernel_ebp = (kernel_ebp - (int)current()) + (int)(childUnion);
	childUnion->task.kernel_esp = kernel_ebp + sizeof(unsigned long);

	unsigned long aux_ebp =*(unsigned long*)kernel_ebp;

	childUnion->task.kernel_esp -= sizeof(unsigned long);
	*(unsigned long*)(childUnion->task.kernel_esp) = (unsigned long)&ret_from_fork;
	childUnion->task.kernel_esp -= sizeof(unsigned long);
	*(unsigned long*)(childUnion->task.kernel_esp) = aux_ebp;

	list_add_tail(&(childUnion->task.list), &readyqueue);

  return PID;
}

void sys_exit()
{  
	int i; 
	page_table_entry *processPT = get_PT(current());

	for(i=0; i<NUM_PAG_DATA;++i) {
		free_frame(get_frame(processPT,PAG_LOG_INIT_DATA+i));
		del_ss_pag(processPT,PAG_LOG_INIT_DATA+i);
	}
	
	list_add_tail(&(current()->list),&freequeue);
	current()->PID=-1;

	sched_next_rr();
	
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

extern int remaining_quantum;

int sys_get_stats(int pid, struct stats *st) {
	int i;
	if (pid < 0) return -EINVAL;
	for(i=0;i<NR_TASKS;++i) {
		if (task[i].task.PID == pid) {
			task[i].task.process_stats.remaining_ticks = remaining_quantum;
			copy_to_user(&(task[i].task.process_stats),st,sizeof(struct stats));
			return 0;
		}
	}
	return -ESRCH;
}
