#include "instruction.h"
#include <Windows.h>

int process_number = 0;	//进程数量
std::vector<process*> ready_queue;		//就绪队列
std::vector<process*> finished_queue;	//结束队列
void ScheduleProcess();
void SimulateInstruction(process* running_process);

//进行处理机进程调度
void ScheduleProcess()
{
	process* running_process;
	while (!ready_queue.empty() && (finished_queue.size() < process_number))
	{
		//随机选择一个就绪线程运行
		srand((unsigned)time(NULL));
		int rand_num = rand() % ready_queue.size();
		running_process = ready_queue[rand_num];
		ready_queue.erase(ready_queue.begin() + rand_num);
		running_process->PCB.process_state = RUNNING;
		std::cout << "当前运行进程为：" << running_process->PCB.process_name << std::endl;
		PC = running_process->PCB.pause;
		SimulateInstruction(running_process);
		running_process->PCB.pause = PC;
		std::cout << std::endl;
		Sleep(500);
	}

	std::cout << "程序结束" << std::endl;
}

void SimulateInstruction(process* running_process)
{
	bool is_waiting = false;	//标记当前进程是否被加入到等待队列中
	std::any instruction;
	//如果现行进程为生产者
	if (running_process->is_producer)
	{
		instruction = PA[PC];
	}
	else
	{
		instruction = SA[PC];
	}

	PC++;
	bool res;	//存放p，v操作后返回的布尔值
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
			std::cout << "当前进程请求empty资源" << std::endl;
			res = std::any_cast<fun>(instruction)(s1);
			if (res)
			{
				is_waiting = true;
				running_process->PCB.process_state = WAITING;
				running_process->PCB.wait_reason = "empty资源不足";
				s1_wait_queue.push(running_process);	//将当前进程插入到s1信号量等待队列中
				std::cout << running_process->PCB.wait_reason << std::endl;
				std::cout << "将当前进程插入到s1信号量等待队列中" << std::endl;
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
			std::cout << "当前进程释放full资源" << std::endl;
			res = std::any_cast<fun>(instruction)(s2);
			if (res)
			{
				process* ready_process = s2_wait_queue.front();
				s2_wait_queue.pop();
				ready_process->PCB.process_state = READY;
				ready_queue.push_back(ready_process);
				std::cout << "进程" << ready_process->PCB.process_name << "从s2信号量等待队列中被唤醒" << std::endl;
			}
			running_process->PCB.process_state = READY;
			Sleep(500);
			break;
		case 4:
			std::any_cast<func>(instruction)(0);	//goto 0
			std::cout << running_process->PCB.process_name << "进程" << "执行goto 0指令" << std::endl;
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
			std::cout << "当前进程请求full资源" << std::endl;
			res = std::any_cast<fun>(instruction)(s2);
			if (res)
			{
				is_waiting = true;
				running_process->PCB.process_state = WAITING;
				running_process->PCB.wait_reason = "full资源不足";
				s2_wait_queue.push(running_process);	//将当前进程插入到s2信号量等待队列中
				std::cout << running_process->PCB.wait_reason << std::endl;
				std::cout << "将当前进程插入到s2信号量等待队列中" << std::endl;
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
			std::cout << "当前进程释放empty资源" << std::endl;
			res = std::any_cast<fun>(instruction)(s1);
			if (res)
			{
				process* ready_process = s1_wait_queue.front();
				s1_wait_queue.pop();
				ready_process->PCB.process_state = READY;
				ready_queue.push_back(ready_process);
				std::cout << "进程" << ready_process->PCB.process_name << "从s1信号量等待队列中被唤醒" << std::endl;
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
			std::cout << running_process->PCB.process_name << "进程" << "执行goto 0指令" << std::endl;
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

	//此时询问是否将进程置为完成态
	if (PC == 4)
	{
		std::string input;
		std::cout << "下一条指令为goto 0，是否结束进程？[y/n]" << std::endl;
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
	//初始化信号量s1，s2
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

	p1->is_producer = true;	//将该进程置为生产者进程
	PC = 0;
	ScheduleProcess();
	delete s1;
	delete s2;
	delete p1;
	delete p2;
	system("pause");
	return 0;
}