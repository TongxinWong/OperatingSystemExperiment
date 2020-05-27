#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>
#include <Windows.h>
#include "partition.h"
using namespace std;
int current_largest_id = 5;	//记录当前空闲分区表中最大的分区号，用于回收分区向空闲分区表中插入新的分区

//以" "空格为分界分割字符串，处理文本文件中的分区信息
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

//向空闲分区表中加入分区，按照起始地址决定插入位置，由链表实现
void AddPartition(partition* free_list, partition* new_partition)
{
	partition* tmp_partition = free_list;
	int check_number = new_partition->start_address;

	//寻找插入位置
	while (tmp_partition->next_partition != NULL && check_number >= tmp_partition->next_partition->start_address)
	{
		tmp_partition = tmp_partition->next_partition;
	}

	//将分区插入空闲分区表
	new_partition->next_partition = tmp_partition->next_partition;
	tmp_partition->next_partition = new_partition;
}

//向已分配分区表中插入已分配分区，插到链表表尾
void AddAllocatedPartition(allocated_partition* allocated_list, allocated_partition* new_allocated_partition)
{
	allocated_partition* tmp = allocated_list;
	while (tmp->next_partition != NULL)
	{
		tmp = tmp->next_partition;
	}
	tmp->next_partition = new_allocated_partition;
}

//查询回收分区是否上邻接空闲分区
partition* FindOverAdjacency(partition* free_list, allocated_partition* p)
{
	partition* tmp = free_list->next_partition;
	while (tmp != NULL)
	{
		if ((tmp->start_address + tmp->length) == p->start_address)
		{
			break;
		}
		tmp = tmp->next_partition;
	}
	return tmp;
}

//查询回收分区是否下邻接空闲分区
partition* FindUnderAdjacency(partition* free_list, allocated_partition* p)
{
	partition* tmp = free_list->next_partition;
	while (tmp != NULL)
	{
		if ((p->start_address + p->length) == tmp->start_address)
		{
			break;
		}
		tmp = tmp->next_partition;
	}
	return tmp;
}

//主存分配
bool Allocate(partition* free_list, allocated_partition* allocated_list, string pid, int length)
{
	if (length == 0)
	{
		return false;
	}

	partition* pre = free_list;
	partition* tmp = pre->next_partition;

	while (tmp != NULL)
	{
		if (length <= tmp->length)
		{
			break;
		}

		pre = tmp;
		tmp = pre->next_partition;
	}

	if (tmp == NULL)
	{
		return false;
	}

	int new_length = tmp->length - length;
	tmp->length = new_length;

	allocated_partition* p;
	p = new allocated_partition;
	p->pid = pid;
	p->start_address = tmp->start_address + new_length;
	p->length = length;
	p->next_partition = NULL;
	AddAllocatedPartition(allocated_list, p);
	if (new_length == 0)
	{
		pre->next_partition = tmp->next_partition;
		delete tmp;
	}
	return true;
}

//主存回收
bool Recycle(allocated_partition* allocated_list, partition* free_list, string pid)
{
	allocated_partition* pre = allocated_list;
	allocated_partition* tmp = pre->next_partition;

	while (tmp != NULL)
	{
		if (tmp->pid == pid)
		{
			break;
		}

		pre = tmp;
		tmp = pre->next_partition;
	}

	if (tmp == NULL)
	{
		return false;
	}

	partition* over = FindOverAdjacency(free_list, tmp);
	partition* under = FindUnderAdjacency(free_list, tmp);

	if (over != NULL)
	{
		if (under != NULL)
		{
			//回收分区上下各邻接一个空闲分区
			over->length += tmp->length;
			over->length += under->length;
			pre->next_partition = tmp->next_partition;
			delete tmp;
			over->next_partition = under->next_partition;
			delete under;
		}
		else
		{
			//回收分区上面邻接一个空闲分区
			over->length += tmp->length;
			pre->next_partition = tmp->next_partition;
			delete tmp;
		}
	}
	else
	{
		if (under != NULL)
		{
			//回收分区下面邻接一个空闲分区
			under->length += tmp->length;
			under->start_address = tmp->start_address;
			pre->next_partition = tmp->next_partition;
			delete tmp;
		}
		else
		{
			//回收分区不与任何空闲分区相邻
			partition* p;
			p = new partition;
			current_largest_id++;
			p->id = current_largest_id;
			p->start_address = tmp->start_address;
			p->length = tmp->length;
			p->next_partition = NULL;
			AddPartition(free_list, p);
			pre->next_partition = tmp->next_partition;
			delete tmp;
		}
	}
	return true;
}

//显示空闲分区表
void DisplayList(partition* list)
{
	cout << "分区号 | 起始地址 | 大小" << endl;
	partition* tmp = list->next_partition;
	while (tmp != NULL)
	{
		cout << setw(4) << tmp->id << "  " <<
			setw(8) << tmp->start_address << "  " <<
			setw(7) << tmp->length << endl;
		tmp = tmp->next_partition;
	}
	cout << endl;
}
//显示已分配分区表
void DisplayList(allocated_partition* list)
{
	cout << "进程号 | 起始地址 | 大小" << endl;
	allocated_partition* tmp = list->next_partition;
	while (tmp != NULL)
	{
		cout << setw(4) << tmp->pid << "  " <<
			setw(8) << tmp->start_address << "  " <<
			setw(7) << tmp->length << endl;
		tmp = tmp->next_partition;
	}
	cout << endl;
}

//释放内存
void FreeMemory(partition* list)
{
	partition* pre = list;
	partition* p = pre->next_partition;

	while (p != NULL)
	{
		delete pre;
		pre = p;
		p = pre->next_partition;
	}

	delete pre;
}

void FreeMemory(allocated_partition* list)
{
	allocated_partition* pre = list;
	allocated_partition* p = pre->next_partition;

	while (p != NULL)
	{
		delete pre;
		pre = p;
		p = pre->next_partition;
	}

	delete pre;
}

int main()
{
	partition* free_partition_list;				//空闲分区表头指针
	allocated_partition* allocated_partition_list;	//已分配分区表头指针
	free_partition_list = new partition;
	allocated_partition_list = new allocated_partition;
	free_partition_list->next_partition = NULL;
	allocated_partition_list->next_partition = NULL;

	ifstream initial_file("initial_partitions.txt");
	string partition_line;
	vector<string> partition_info;
	if (initial_file)
	{
		while (getline(initial_file, partition_line))
		{
			partition_info.clear();
			SplitString(partition_line, partition_info, " ");
			partition* tmp_partition = new partition;
			tmp_partition->id = stoi(partition_info[0]);
			tmp_partition->start_address = stoi(partition_info[1]);
			tmp_partition->length = stoi(partition_info[2]);
			AddPartition(free_partition_list, tmp_partition);	//将每一行的分区加入到空闲分区
		}
	}
	else
	{
		cout << "没有初始化文件！" << endl;
	}
	initial_file.close();

	cout << "初始空闲分区表：" << endl;
	DisplayList(free_partition_list);

	string input;
	getline(cin, input);
	while (input != "Q" && input != "q" && input != "")
	{
		vector<string> request_info;
		SplitString(input, request_info, " ");

		if (request_info[0] == "allocate")
		{
			bool res = Allocate(free_partition_list, allocated_partition_list, request_info[1], stoi(request_info[2]));
			if (res)
			{
				cout << "分配完成后当前状态" << endl;
				cout << "空闲分区表：" << endl;
				DisplayList(free_partition_list);
				cout << "已分配分区表：" << endl;
				DisplayList(allocated_partition_list);
			}
			else
			{
				cout << "Allocate failed!" << endl;
			}
		}
		else if (request_info[0] == "recycle")
		{
			bool res = Recycle(allocated_partition_list, free_partition_list, request_info[1]);
			if (res)
			{
				cout << "回收完成后当前状态" << endl;
				cout << "空闲分区表：" << endl;
				DisplayList(free_partition_list);
				cout << "已分配分区表：" << endl;
				DisplayList(allocated_partition_list);
			}
			else
			{
				cout << "Recycle failed!" << endl;
			}
		}
		else
		{
			cout << "Error: Invaild command!" << endl;
		}
		getline(cin, input);
	}

	FreeMemory(free_partition_list);
	FreeMemory(allocated_partition_list);
	return 0;
}