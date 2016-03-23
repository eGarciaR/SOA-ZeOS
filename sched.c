/*
 * sched.c - initializes struct for task 0 anda task 1
 */

#include <sched.h>
#include <mm.h>
#include <io.h>

/**
 * Container for the Task array and 2 additional pages (the first and the last one)
 * to protect against out of bound accesses.
 */
union task_union protected_tasks[NR_TASKS+2]
  __attribute__((__section__(".data.task")));

union task_union *task = &protected_tasks[1]; /* == union task_union task[NR_TASKS] */

struct list_head freequeue; /*Free queue*/
struct list_head readyqueue; /*Ready queue*/
struct task_struct *idle_task; /*idle task*/

struct task_struct *list_head_to_task_struct(struct list_head *l)
{
  return list_entry( l, struct task_struct, list);
}

extern struct list_head blocked;


/* get_DIR - Returns the Page Directory address for task 't' */
page_table_entry * get_DIR (struct task_struct *t) 
{
	return t->dir_pages_baseAddr;
}

/* get_PT - Returns the Page Table address for task 't' */
page_table_entry * get_PT (struct task_struct *t) 
{
	return (page_table_entry *)(((unsigned int)(t->dir_pages_baseAddr->bits.pbase_addr))<<12);
}


int allocate_DIR(struct task_struct *t) 
{
	int pos;

	pos = ((int)t-(int)task)/sizeof(union task_union);

	t->dir_pages_baseAddr = (page_table_entry*) &dir_pages[pos]; 

	return 1;
}

void cpu_idle(void)
{
	__asm__ __volatile__("sti": : :"memory");

	while(1)
	{
	;
	}
}

void init_idle (void)
{
	struct list_head *firstFree = list_first(&freequeue);
	list_del(firstFree); // Esborrem l'element de la cua que volem utilitzar
	idle_task = list_head_to_task_struct(firstFree);

	union task_union * idle_task_union = (union task_union *)idle_task;

	idle_task_union->task.PID = 0;
	allocate_DIR(&idle_task_union->task);
	idle_task_union->stack[1023] = (unsigned long)cpu_idle; //(unsigned long)&cpu_idle;
	idle_task_union->stack[1022] = 0; /*ebp*/
	idle_task_union->task.init_ebp = (unsigned long)&(idle_task_union->stack[1022]);

}

void init_task1(void)
{	
  struct list_head *firstFree = list_first(&freequeue);
  list_del(firstFree);
  struct task_struct *init_task1 = list_head_to_task_struct(firstFree);
  
  union task_union * init_task1_union = (union task_union *)init_task1;
  
  init_task1_union->task.PID = 0;
  allocate_DIR(&init_task1_union->task);
  set_user_pages(&init_task1_union->task);

	tss.esp0 = (unsigned long)&init_task1_union->stack[KERNEL_STACK_SIZE];
	
	page_table_entry * dir_task1 = get_DIR(&init_task1_union->task);
	set_cr3(dir_task1);
}


void init_sched(){
  /*FREE QUEUE*/
  INIT_LIST_HEAD( &freequeue );
  
  int i;
  for (i = 0; i < NR_TASKS; ++i) {
    list_add_tail( &(task[i].task.list), &freequeue );
  }
  
  /*READY QUEUE*/
  INIT_LIST_HEAD( &readyqueue );
  
}

void inner_task_switch(union task_union*t) {

	tss.esp0 = (unsigned long)&t->stack[KERNEL_STACK_SIZE];	
	
	page_table_entry * dir_task_switch = get_DIR(&t->task);
	set_cr3(dir_task1);

	// 3, 4, 5 i 6
	
}

void task_switch(union task_union*t) {
	/*SAVE esi, edi and ebx*/
	asm("pushl %esi;"
			"pushl %edi;"
			"pushl %ebx"
	);

	inner_task_switch(t);	

	/*RESTORE esi, edi and ebx*/
	asm("popl %esi;"
			"popl %edi;"
			"popl %ebx"
	);
}

struct task_struct* current()
{
  int ret_value;
  
  __asm__ __volatile__(
  	"movl %%esp, %0"
	: "=g" (ret_value)
  );
  return (struct task_struct*)(ret_value&0xfffff000);
}

