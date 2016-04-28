#include <libc.h>
#include <schedperf.h>

char buff[512];

int pid;

int set_sched_policy(int);

int __attribute__ ((__section__(".text.main")))
  main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */
	
  set_sched_policy(0);
	//struct stats st;
	//get_stats(getpid(),&st);
	//itoa(st.user_ticks, buff);
	//write(1,buff,strlen(buff));
	//write(1,"\n",1);
	fork();
	fork();
	//fork();
  //runjp();
	/*int child[5];
	int i;
	for (i=0;i<5;++i) {
		child[i]=fork();
		if (child[i] >0) {
		}
		else if (child[i]==0) {
			struct stats st;
			int j = 0;
			while(j<1000) {
				++j;
				//write(1,"\n",1);
			}
			get_stats(getpid(),&st);
			itoa(st.user_ticks, buff);
			write(1,"hijo:",7);
			write(1,buff,strlen(buff));
			write(1,"\n",1);
			break;
		}	
		else if (child[i] < 0) {
			write(1,"UUUUU",5);
			perror();
			exit();
		}
	}*/
  while(1) {
		itoa(getpid(),buff);
    write(1,buff,strlen(buff));
    write(1,"\n",strlen("\n"));
	  //itoa(gettime(),buff);
    //write(1,buff,strlen(buff));	
		//write(1,"\n",strlen("\n"));
	};
  return 0;
}
