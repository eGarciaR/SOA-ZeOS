#include <libc.h>

char buff[24];

int pid;

int __attribute__ ((__section__(".text.main")))
  main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */
	
  int res;
  res = set_sched_policy(0);
  runjp();
  while(1) {
    //itoa(getpid(),buff);
    //write(1,buff,strlen(buff));
    //write(1,"\n",strlen("\n"));
	//itoa(gettime(),buff);
  //write(1,buff,strlen(buff));	
	//write(1,"\n",strlen("\n"));
	};
  return 0;
}
