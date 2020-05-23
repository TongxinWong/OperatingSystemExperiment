#pragma once
#include <iostream>
#include <time.h>
#include <any>
#include <queue>
#include <string>
#include <stdlib.h>
#include <random>

//READY-������RUNNING-���У�WAITING-�ȴ���FINISHED-����
enum ProcessState { READY, RUNNING, WAITING, FINISHED };

typedef struct
{
	std::string process_name;
	ProcessState process_state;
	std::string wait_reason;
	int pause;	//�ϵ�
}PCB;

typedef struct
{
	bool is_producer = false;	//����Ƿ�Ϊ�����߽��̣�Ĭ��Ϊfalse
	PCB PCB;
}process;

typedef int semaphore;
semaphore* s1;		//�ź���s1������empty
semaphore* s2;		//�ź���s2������full
std::queue<process*> s1_wait_queue;	//�ź���s1�ȴ�����
std::queue<process*> s2_wait_queue;	//�ź���s2�ȴ�����
char buffer[10] = {' '};	//�����
int IN = 0;				//�����ߴ�Ų�Ʒ�����ָ��
int OUT = 0;			//���������Ѳ�Ʒ�����ָ��
char c;					//��������������Ĳ�Ʒ
char x;					//������������ѵĲ�Ʒ
int PC;					//ָ�������

//��������ͺ���ָ�룬ģ��ָ������
typedef void(*action)(void);	//�޲��޷���ֵ
typedef bool(*fun)(semaphore*);	//����Ϊsemaphore������ֵΪ�������������ж��Ƿ���Ҫ�����̲�����Ƴ��ȴ����У�p��v����
typedef void(*func)(int);		//����Ϊint�޷���ֵ��gotoָ��

bool p(semaphore* s)
{
	std::cout << "ϵͳ����Դ����Ϊ" << *s << std::endl;
	(*s)--;
	std::cout << "�����ϵͳ����Դ����Ϊ" << *s << std::endl;
	if (*s < 0)
	{
		return true;
	}
	return false;
}

bool v(semaphore* s)
{
	std::cout << "�ͷ�ǰϵͳ����Դ����Ϊ" << *s << std::endl;
	(*s)++;
	std::cout << "�ͷź�ϵͳ����Դ����Ϊ" << *s << std::endl;
	if (*s <= 0)
	{
		return true;
	}
	return false;
}

void PUT()
{
	std::cout << "�����߽���Ʒ" << c << "�����˻������" << std::endl;
	buffer[IN] = c;
	IN = (IN + 1) % 10;
	std::cout << "��ǰ�����Ϊ" << std::endl;
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
	std::cout << "�����ߴӻ���ػ����" << x << "��Ʒ" << std::endl;
	OUT = (OUT + 1) % 10;
	std::cout << "��ǰ�����Ϊ" << std::endl;
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
	//��A-Z���������һ���ַ�
	c = 'A' + rand_num;
	std::cout << "������������" << c << std::endl;
}

void consume()
{
	std::cout << "������������" << x << "��Ʒ" << std::endl;
}

void GOTO(int address)
{
	PC = address;
}

std::any PA[5] = { produce,p,PUT,v,GOTO };	//��������߳���ģ��ָ������
std::any SA[5] = { p,GET,v,consume,GOTO };	//��������߳���ģ��ָ������