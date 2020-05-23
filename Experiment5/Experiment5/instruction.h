#pragma once
#include <iostream>
#include <time.h>
#include <any>
#include <queue>
#include <string>
#include <stdlib.h>
#include <random>

//READY-就绪，RUNNING-运行，WAITING-等待，FINISHED-结束
enum ProcessState { READY, RUNNING, WAITING, FINISHED };

typedef struct
{
	std::string process_name;
	ProcessState process_state;
	std::string wait_reason;
	int pause;	//断点
}PCB;

typedef struct
{
	bool is_producer = false;	//标记是否为生产者进程，默认为false
	PCB PCB;
}process;

typedef int semaphore;
semaphore* s1;		//信号量s1，代表empty
semaphore* s2;		//信号量s2，代表full
std::queue<process*> s1_wait_queue;	//信号量s1等待队列
std::queue<process*> s2_wait_queue;	//信号量s2等待队列
char buffer[10] = {' '};	//缓冲池
int IN = 0;				//生产者存放产品缓冲池指针
int OUT = 0;			//消费者消费产品缓冲池指针
char c;					//存放生产者生产的产品
char x;					//存放消费者消费的产品
int PC;					//指令计数器

//定义各类型函数指针，模拟指令类型
typedef void(*action)(void);	//无参无返回值
typedef bool(*fun)(semaphore*);	//参数为semaphore，返回值为布尔类型用于判断是否需要将进程插入和移出等待队列，p，v操作
typedef void(*func)(int);		//参数为int无返回值，goto指令

bool p(semaphore* s)
{
	std::cout << "系统中资源数量为" << *s << std::endl;
	(*s)--;
	std::cout << "请求后系统中资源数量为" << *s << std::endl;
	if (*s < 0)
	{
		return true;
	}
	return false;
}

bool v(semaphore* s)
{
	std::cout << "释放前系统中资源数量为" << *s << std::endl;
	(*s)++;
	std::cout << "释放后系统中资源数量为" << *s << std::endl;
	if (*s <= 0)
	{
		return true;
	}
	return false;
}

void PUT()
{
	std::cout << "生产者将产品" << c << "放入了缓冲池中" << std::endl;
	buffer[IN] = c;
	IN = (IN + 1) % 10;
	std::cout << "当前缓冲池为" << std::endl;
	std::cout << "|";
	for (int i = 0; i < 10; i++)
	{
		if (buffer[i] == ' ')
		{
			std::cout << ' ' << "|";
		}
		else
		{
			std::cout << buffer[i] << "|";
		}
	}
	std::cout << std::endl;
}

void GET()
{
	x = buffer[OUT];
	buffer[OUT] = ' ';
	std::cout << "消费者从缓冲池获得了" << x << "产品" << std::endl;
	OUT = (OUT + 1) % 10;
	std::cout << "当前缓冲池为" << std::endl;
	for (int i = 0; i < 10; i++)
	{
		if (buffer[i] == ' ')
		{
			std::cout << ' ' << "|";
		}
		else
		{
			std::cout << buffer[i] << "|";
		}
	}
	std::cout << std::endl;
}

void produce()
{
	srand((unsigned)time(NULL));
	int rand_num = rand() % 25;
	//从A-Z中随机产生一个字符
	c = 'A' + rand_num;
	std::cout << "生产者生产了" << c << std::endl;
}

void consume()
{
	std::cout << "消费者消费了" << x << "产品" << std::endl;
}

void GOTO(int address)
{
	PC = address;
}

std::any PA[5] = { produce,p,PUT,v,GOTO };	//存放生产者程序模拟指令序列
std::any SA[5] = { p,GET,v,consume,GOTO };	//存放消费者程序模拟指令序列