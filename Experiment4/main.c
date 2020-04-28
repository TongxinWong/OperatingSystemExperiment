#include <stdio.h>
#include <unistd.h>

int main()
{
	pid_t pid;
	pid = fork();	//父进程第一次调用fork()创建一个子进程

	if(pid < 0)
	{
		printf("创建子进程失败！\n");
	}
	else if(pid == 0)	//fork()返回0，子进程
	{
		printf("pid: %d 子进程1 显示字符: B\n",getpid());
	}
	else	//父进程
	{
		printf("pid: %d 父进程 显示字符: A\n",getpid());
		pid = fork();	//父进程第二次调用fork()创建一个子进程
		if(pid < 0)
		{
			printf("创建子进程失败！\n");
		}
		else if(pid == 0)
		{
			printf("pid: %d 子进程2 显示字符: C\n",getpid());
		}
	}

	return 0;
}
