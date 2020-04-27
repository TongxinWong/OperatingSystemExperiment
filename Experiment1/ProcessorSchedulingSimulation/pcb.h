#pragma once
#include <string>

//R-就绪，E-结束
enum ProcessState { R, E };
typedef struct node
{
	std::string process_name;
	node* next_process;	//指向下一个PCB的地址
	int running_time;
	int priority_number;
	ProcessState process_state;
}PCB;