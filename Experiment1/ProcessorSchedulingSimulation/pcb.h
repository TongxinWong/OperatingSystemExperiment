#pragma once
#include <string>

//R-������E-����
enum ProcessState { R, E };
typedef struct node
{
	std::string process_name;
	node* next_process;	//ָ����һ��PCB�ĵ�ַ
	int running_time;
	int priority_number;
	ProcessState process_state;
}PCB;