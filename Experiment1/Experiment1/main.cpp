#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>
#include <Windows.h>
#include "pcb.h"
using namespace std;
int process_number = 0;	//进程数量

//以" "空格为分界分割字符串，处理文本文件中的进程信息
void SplitString(const string& s, vector<string>& tokens, const string& delimiter)
{
	string::size_type pos1, pos2;
	pos2 = s.find(delimiter);
	pos1 = 0;
	while (string::npos != pos2)
	{
		tokens.push_back(s.substr(pos1, pos2 - pos1));

		pos1 = pos2 + delimiter.size();
		pos2 = s.find(delimiter, pos1);
	}
	if (pos1 != s.length()) tokens.push_back(s.substr(pos1));
}

//向就绪队列中加入进程，按照优先数决定插入位置，实际上是由链表实现
void AddProcess(PCB* ready_queue, PCB* new_process)
{
	PCB* tmp_process = ready_queue;
	int check_number = new_process->priority_number;

	//寻找插入位置
	while (tmp_process->next_process != NULL && check_number <= tmp_process->next_process->priority_number)
	{
		tmp_process = tmp_process->next_process;
	}

	//将进程插入就绪队列
	new_process->next_process = tmp_process->next_process;
	tmp_process->next_process = new_process;
}

//进行一次进程调度
void ScheduleProcess(PCB* ready_queue, vector<string>& finished_processes)
{
	//就绪队列已经按照优先数进行排序，选取就绪队列的第一个进程
	PCB* running_process = ready_queue->next_process;
	ready_queue->next_process = running_process->next_process;

	cout << "当前正在运行的进程：" << running_process->process_name << "     ";
	cout << "进程就绪队列：";
	PCB* tmp = ready_queue->next_process;
	int printed_process_number = 0;	//计数变量，保存已打印进程名数量，用于格式对齐
	while (tmp != NULL)
	{
		printed_process_number++;
		cout << tmp->process_name << " ";
		tmp = tmp->next_process;
	}
	cout << setw((process_number - printed_process_number) * 3 + 15) << "已结束的进程：";
	for (int i = 0; i < finished_processes.size(); i++)
	{
		cout << finished_processes[i] << " ";
	}
	cout << endl;

	running_process->running_time--;	//要求运行时间减1
	running_process->priority_number--;	//优先数减1
	Sleep(500);

	//若要求运行时间不等于0，则将它加入就绪队列，
	//否则，将状态改为“结束”，退出就绪队列
	if (running_process->running_time != 0)
	{
		AddProcess(ready_queue, running_process);
		return;
	}
	running_process->process_state = E;
	finished_processes.push_back(running_process->process_name);
	delete running_process;	//释放内存
}

int main()
{
	PCB* ready_queue;	//就绪队列头结点指针
	ready_queue = new PCB;
	ready_queue->next_process = NULL;
	int time = 0;	//模拟运行时间
	vector<string> finished_processes;	//已结束的进程

	ifstream initial_file("initial_processes.txt");
	string process_line;
	vector<string> process_info;
	if (initial_file)
	{
		while (getline(initial_file, process_line))
		{
			process_info.clear();
			SplitString(process_line, process_info, " ");
			PCB* pcb = new PCB;
			pcb->process_name = process_info[0];
			pcb->running_time = stoi(process_info[1]);
			pcb->priority_number = stoi(process_info[2]);
			pcb->process_state = R;
			AddProcess(ready_queue, pcb);	//将每一行的进程加入到就绪队列里
			process_number++;
		}
	}
	else
	{
		cout << "没有初始化文件！" << endl;
	}
	initial_file.close();

	cout << "初始进程状态：" << endl;
	cout << "进程名  要求运行时间  优先数" << endl;
	PCB* tmp = ready_queue->next_process;
	while (tmp != NULL)
	{
		cout << setw(4) << tmp->process_name << "  " <<
			setw(8) << tmp->running_time << "  " <<
			setw(10) << tmp->priority_number << endl;
		tmp = tmp->next_process;
	}
	cout << endl;

	while (ready_queue->next_process != NULL)
	{
		cout << setw(2) << time << "：";
		ScheduleProcess(ready_queue, finished_processes);
		time++;
	}

	cout << "END：" << "就绪队列为空 已结束的进程：";
	for (int i = 0; i < finished_processes.size(); i++)
	{
		cout << finished_processes[i] << " ";
	}
	cout << endl;

	delete ready_queue;
	system("pause");
	return 0;
}