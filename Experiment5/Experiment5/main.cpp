#include "instruction.h"
#include <Windows.h>

int process_number = 0;	//��������
std::vector<process*> ready_queue;		//��������
std::vector<process*> finished_queue;	//��������
void ScheduleProcess();
void SimulateInstruction(process* running_process);

//���д�������̵���
void ScheduleProcess()
{
	process* running_process;
	while (!ready_queue.empty() && (finished_queue.size() < process_number))
	{
		//���ѡ��һ�������߳�����
		srand((unsigned)time(NULL));
		int rand_num = rand() % ready_queue.size();
		running_process = ready_queue[rand_num];
		ready_queue.erase(ready_queue.begin() + rand_num);
		running_process->PCB.process_state = RUNNING;
		std::cout << "��ǰ���н���Ϊ��" << running_process->PCB.process_name << std::endl;
		PC = running_process->PCB.pause;
		SimulateInstruction(running_process);
		running_process->PCB.pause = PC;
		std::cout << std::endl;
		Sleep(500);
	}

	std::cout << "�������" << std::endl;
}

void SimulateInstruction(process* running_process)
{
	bool is_waiting = false;	//��ǵ�ǰ�����Ƿ񱻼��뵽�ȴ�������
	std::any instruction;
	//������н���Ϊ������
	if (running_process->is_producer)
	{
		instruction = PA[PC];
	}
	else
	{
		instruction = SA[PC];
	}

	PC++;
	bool res;	//���p��v�����󷵻صĲ���ֵ
	if (running_process->is_producer)
	{
		switch (PC-1)
		{
		case 0:
			std::any_cast<action>(instruction)();
			running_process->PCB.process_state = READY;
			Sleep(500);
			break;
		case 1:
			std::cout << "��ǰ��������empty��Դ" << std::endl;
			res = std::any_cast<fun>(instruction)(s1);
			if (res)
			{
				is_waiting = true;
				running_process->PCB.process_state = WAITING;
				running_process->PCB.wait_reason = "empty��Դ����";
				s1_wait_queue.push(running_process);	//����ǰ���̲��뵽s1�ź����ȴ�������
				std::cout << running_process->PCB.wait_reason << std::endl;
				std::cout << "����ǰ���̲��뵽s1�ź����ȴ�������" << std::endl;
			}
			else
			{
				running_process->PCB.process_state = READY;
			}
			Sleep(500);
			break;
		case 2:
			std::any_cast<action>(instruction)();
			running_process->PCB.process_state = READY;
			Sleep(500);
			break;
		case 3:
			std::cout << "��ǰ�����ͷ�full��Դ" << std::endl;
			res = std::any_cast<fun>(instruction)(s2);
			if (res)
			{
				process* ready_process = s2_wait_queue.front();
				s2_wait_queue.pop();
				ready_process->PCB.process_state = READY;
				ready_queue.push_back(ready_process);
				std::cout << "����" << ready_process->PCB.process_name << "��s2�ź����ȴ������б�����" << std::endl;
			}
			running_process->PCB.process_state = READY;
			Sleep(500);
			break;
		case 4:
			std::any_cast<func>(instruction)(0);	//goto 0
			std::cout << running_process->PCB.process_name << "����" << "ִ��goto 0ָ��" << std::endl;
			running_process->PCB.process_state = READY;
			Sleep(500);
			break;
		default:
			break;
		}
	}
	else
	{
		switch (PC - 1)
		{
		case 0:
			std::cout << "��ǰ��������full��Դ" << std::endl;
			res = std::any_cast<fun>(instruction)(s2);
			if (res)
			{
				is_waiting = true;
				running_process->PCB.process_state = WAITING;
				running_process->PCB.wait_reason = "full��Դ����";
				s2_wait_queue.push(running_process);	//����ǰ���̲��뵽s2�ź����ȴ�������
				std::cout << running_process->PCB.wait_reason << std::endl;
				std::cout << "����ǰ���̲��뵽s2�ź����ȴ�������" << std::endl;
			}
			else
			{
				running_process->PCB.process_state = READY;
			}
			Sleep(500);
			break;
		case 1:
			std::any_cast<action>(instruction)();
			running_process->PCB.process_state = READY;
			Sleep(500);
			break;
		case 2:
			std::cout << "��ǰ�����ͷ�empty��Դ" << std::endl;
			res = std::any_cast<fun>(instruction)(s1);
			if (res)
			{
				process* ready_process = s1_wait_queue.front();
				s1_wait_queue.pop();
				ready_process->PCB.process_state = READY;
				ready_queue.push_back(ready_process);
				std::cout << "����" << ready_process->PCB.process_name << "��s1�ź����ȴ������б�����" << std::endl;
			}
			running_process->PCB.process_state = READY;
			Sleep(500);
			break;
		case 3:
			std::any_cast<action>(instruction)();
			running_process->PCB.process_state = READY;
			Sleep(500);
			break;
		case 4:
			std::any_cast<func>(instruction)(0);	//goto 0
			std::cout << running_process->PCB.process_name << "����" << "ִ��goto 0ָ��" << std::endl;
			running_process->PCB.process_state = READY;
			Sleep(500);
			break;
		default:
			break;
		}
	}

	if (is_waiting)
	{
		return;
	}

	//��ʱѯ���Ƿ񽫽�����Ϊ���̬
	if (PC == 4)
	{
		std::string input;
		std::cout << "��һ��ָ��Ϊgoto 0���Ƿ�������̣�[y/n]" << std::endl;
		std::cin >> input;
		if (input == "y" || input == "Y")
		{
			running_process->PCB.process_state = FINISHED;
			finished_queue.push_back(running_process);
			return;
		}
	}

	ready_queue.push_back(running_process);
}

int main()
{
	//��ʼ���ź���s1��s2
	s1 = new semaphore;
	s2 = new semaphore;
	*s1 = 10;
	*s2 = 0;
	process* p1;
	process* p2;
	p1 = new process;
	p2 = new process;

	p1->PCB.process_name = "producer";
	p1->PCB.process_state = READY;
	p1->PCB.pause = 0;
	ready_queue.push_back(p1);
	process_number++;

	p2->PCB.process_name = "consumer";
	p2->PCB.process_state = READY;
	p2->PCB.pause = 0;
	ready_queue.push_back(p2);
	process_number++;

	p1->is_producer = true;	//���ý�����Ϊ�����߽���
	PC = 0;
	ScheduleProcess();
	delete s1;
	delete s2;
	delete p1;
	delete p2;
	system("pause");
	return 0;
}