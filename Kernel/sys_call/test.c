#include <syscall.h>
#include <stdio.h>
#include <unistd.h>
int main(void)
{
	printf("%ld\n",syscall(333));
	return 0;
}

