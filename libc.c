/*
 * libc.c 
 */

#include <libc.h>
#include <errno.h>
#include <types.h>

int errno;
int res;

void itoa(int a, char *b)
{
  int i, i1;
  char c;
  
  if (a==0) { b[0]='0'; b[1]=0; return ;}
  
  i=0;
  while (a>0)
  {
    b[i]=(a%10)+'0';
    a=a/10;
    i++;
  }
  
  for (i1=0; i1<i/2; i1++)
  {
    c=b[i1];
    b[i1]=b[i-i1-1];
    b[i-i1-1]=c;
  }
  b[i]=0;
}

int strlen(char *a)
{
  int i;
  
  i=0;
  
  while (a[i]!=0) i++;
  
  return i;
}

int write (int fd, char * buffer, int size) {
	asm("pushl %ebx;"
			"movl 8(%ebp),%ebx;"
			"movl 12(%ebp),%ecx;"
			"movl 16(%ebp),%edx;"
			"movl $4,%eax;"
			"int $0x80;"
			"popl %ebx;"
			"movl %eax, res"
	);
	
	if (res < 0) {
		errno = -res;
		res = -1;
	}
	return res;
}

int gettime () {
	asm("movl $10,%eax;"
			"int $0x80;"
			"movl %eax, res"
	);
	
	if (res < 0) {
		errno = -res;
		res = -1;
	}
	return res;
}

void perror() {
	if (errno == EFAULT) write(1,"Bad address",strlen("Bad address"));
	else if (errno == EINVAL) write(1,"Invalid argument",strlen("Invalid argument"));
	else write(1,"Error",strlen("Error"));
	write (1,"\n",1);
}

