#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>
#include <Windows.h>
#include "pcb.h"
using namespace std;
int process_number = 0;	//��������

//��" "�ո�Ϊ�ֽ�ָ��ַ����������ı��ļ��еĽ�����Ϣ
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

//����������м�����̣�������������������λ�ã�ʵ������������ʵ��
void AddProcess(PCB* ready_queue, PCB* new_process)
{
	PCB* tmp_process = ready_queue;
	int check_number = new_process->priority_number;

	//Ѱ�Ҳ���λ��
	while (tmp_process->next_process != NULL && check_number <= tmp_process->next_process->priority_number)
	{
		tmp_process = tmp_process->next_process;
	}

	//�����̲����������
	new_process->next_process = tmp_process->next_process;
	tmp_process->next_process = new_process;
}

//����һ�ν��̵���
void ScheduleProcess(PCB* ready_queue, vector<string>& finished_processes)
{
	//���������Ѿ�������������������ѡȡ�������еĵ�һ������
	PCB* running_process = ready_queue->next_process;
	ready_queue->next_process = running_process->next_process;

	cout << "��ǰ�������еĽ��̣�" << running_process->process_name << "     ";
	cout << "���̾������У�";
	PCB* tmp = ready_queue->next_process;
	int printed_process_number = 0;	//���������������Ѵ�ӡ���������������ڸ�ʽ����
	while (tmp != NULL)
	{
		printed_process_number++;
		cout << tmp->process_name << " ";
		tmp = tmp->next_process;
	}
	cout << setw((process_number - printed_process_number) * 3 + 15) << "�ѽ����Ľ��̣�";
	for (int i = 0; i < finished_processes.size(); i++)
	{
		cout << finished_processes[i] << " ";
	}
	cout << endl;

	running_process->running_time--;	//Ҫ������ʱ���1
	running_process->priority_number--;	//��������1
	Sleep(500);

	//��Ҫ������ʱ�䲻����0����������������У�
	//���򣬽�״̬��Ϊ�����������˳���������
	if (running_process->running_time != 0)
	{
		AddProcess(ready_queue, running_process);
		return;
	}
	running_process->process_state = E;
	finished_processes.push_back(running_process->process_name);
	delete running_process;	//�ͷ��ڴ�
}

int main()
{
	PCB* ready_queue;	//��������ͷ���ָ��
	ready_queue = new PCB;
	ready_queue->next_process = NULL;
	int time = 0;	//ģ������ʱ��
	vector<string> finished_processes;	//�ѽ����Ľ���

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
			AddProcess(ready_queue, pcb);	//��ÿһ�еĽ��̼��뵽����������
			process_number++;
		}
	}
	else
	{
		cout << "û�г�ʼ���ļ���" << endl;
	}
	initial_file.close();

	cout << "��ʼ����״̬��" << endl;
	cout << "������  Ҫ������ʱ��  ������" << endl;
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
		cout << setw(2) << time << "��";
		ScheduleProcess(ready_queue, finished_processes);
		time++;
	}

	cout << "END��" << "��������Ϊ�� �ѽ����Ľ��̣�";
	for (int i = 0; i < finished_processes.size(); i++)
	{
		cout << finished_processes[i] << " ";
	}
	cout << endl;

	delete ready_queue;
	system("pause");
	return 0;
}