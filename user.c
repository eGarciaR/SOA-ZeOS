#include <libc.h>
#include <schedperf.h>

char buff[512];
char buffAux[512];

int pid;

int set_sched_policy(int);
int read(int,char*,int);

int __attribute__ ((__section__(".text.main")))
  main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */
	
  set_sched_policy(0);
  //runjp();
  int pids[3];
  int i;
  for (i=0;i<3;++i) {
	  int j = 0;
	  pids[i]=fork();
	  if (pids[i] == 0) {
		  //write(1,"ABCDE",strlen("ABCDE"));
		  //write(1,"\n",1);
		  struct stats st;
		  read(1,buff,5);
		  while (j<1000) {
			  ++j;
			  read(0,buff,5);
		  }
		  j = 0;
		  while (j<10000) {
			  ++j;
		  }
		  get_stats(getpid(),&st);
		  itoa(st.user_ticks, buff);
		  itoa(getpid(),buffAux);
		  write(1,buffAux,strlen(buffAux));
		  write(1,"user_ticks: ",strlen("user_ticks: "));
		  write(1,buff,strlen(buff));
		  write(1,"\n",1);

		  itoa(st.system_ticks, buff);
		  itoa(getpid(),buffAux);
		  write(1,buffAux,strlen(buffAux));
		  write(1,"system_ticks: ",strlen("system_ticks: "));
		  write(1,buff,strlen(buff));
		  write(1,"\n",1);

		  itoa(st.blocked_ticks, buff);
		  itoa(getpid(),buffAux);
		  write(1,buffAux,strlen(buffAux));
		  write(1,"blocked_ticks: ",strlen("blocked_ticks: "));
		  write(1,buff,strlen(buff));
		  write(1,"\n",1);

		  itoa(st.ready_ticks, buff);
		  itoa(getpid(),buffAux);
		  write(1,buffAux,strlen(buffAux));
		  write(1,"ready_ticks: ",strlen("ready_ticks: "));
		  write(1,buff,strlen(buff));
		  write(1,"\n",1);

		  exit();
	  }
	  else if (pids[i] < 0) {
		  perror();
		  exit();
	  }
  }
  // 00010820 <task_switch>:  00010a40 <block_process>:   00010a80 <unblock_process>:
  //read(0,buff,100);
  while(1) {
	//fork();
	//fork();
	//itoa(getpid(),buff);
	//write(1,buff,strlen(buff));
	//write(1,"\n",strlen("\n"));
	//itoa(gettime(),buff);
	//write(1,buff,strlen(buff));	
	//write(1,"\n",strlen("\n"));
  };
  return 0;
}
