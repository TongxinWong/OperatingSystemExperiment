#include <iostream>
#include <iomanip>
#include <string>
#include "file.h"
using namespace std;

//以" "空格为分界分割字符串，处理申请和释放请求
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

//申请分配空间
bool Allocate(vector<file_info>& file_list, int digital_map[8][8], string file_name, int block_number)
{
	if (block_number == 0)
	{
		return false;
	}

	vector<int> allocated_blocks;	//记录已分配的物理块，如果文件所需物理块数目超过剩余物理块，则将已分配的物理块重新回收
	file_info new_file;
	new_file.file_name = file_name;
	new_file.block_number = block_number;
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			if (digital_map[i][j])
			{
				continue;
			}

			digital_map[i][j] = 1;
			allocated_blocks.push_back(i * 8 + j);
			int cylinder_number = i;	//柱⾯号 = 字节号
			int track_number = j / 4;	//磁道号 = 位数 / 4
			int sector_number = j % 4;	//物理记录号 = 位数 % 4

			physical_address address;
			address.cylinder_number = cylinder_number;
			address.track_number = track_number;
			address.sector_number = sector_number;
			new_file.physical_blocks.push_back(address);

			block_number--;
			if (block_number == 0)
			{
				break;
			}
		}

		if (block_number == 0)
		{
			break;
		}
	}

	if (block_number != 0)
	{
		//将已分配的物理块回收
		for (int i = 0; i < allocated_blocks.size(); i++)
		{
			int number = allocated_blocks[i];
			digital_map[number / 8][number % 8] = 0;
		}
		return false;
	}

	cout << "分配到的磁盘空间物理地址：" << endl;
	for (int i = 0; i < new_file.physical_blocks.size(); i++)
	{
		physical_address address = new_file.physical_blocks[i];
		int cylinder_number = address.cylinder_number;
		int track_number = address.track_number;
		int sector_number = address.sector_number;
		cout << setw(2) << cylinder_number * 8 + track_number * 4 + sector_number << "| " <<
			"柱面号：" << cylinder_number << " " <<
			"磁道号：" << track_number << " " <<
			"物理记录号：" << sector_number << " " << endl;
	}
	cout << endl;

	file_list.push_back(new_file);	//向文件列表插入文件信息
	return true;
}

//申请释放空间
bool Recycle(vector<file_info>& file_list, int digital_map[8][8], string file_name)
{
	file_info* old_file;
	old_file = NULL;

	vector<file_info>::iterator it;
	for (it = file_list.begin(); it != file_list.end(); it++)
	{
		if (it->file_name == file_name)
		{
			old_file = &(*it);
			break;
		}

		continue;
	}

	if (old_file == NULL)
	{
		return false;
	}

	//将已分配的物理块回收
	cout << "归还块对应于位示图的字节号和位数：" << endl;
	for (int i = 0; i < old_file->physical_blocks.size(); i++)
	{
		physical_address address = old_file->physical_blocks[i];
		int m = address.cylinder_number;	//字节号 = 柱⾯号
		int n = address.track_number * 4 + address.sector_number;	//位数 = 磁道号*4 + 物理记录号
		digital_map[m][n] = 0;
		cout << setw(2) << m * 8 + n << "| " << "字节号：" << m << " 位数：" << n << endl;
	}
	cout << endl;

	file_list.erase(it);	//从文件列表删除文件信息
	return true;
}

//显示位示图
void DisplayDigitalMap(int digital_map[8][8])
{
	cout << "位示图：" << endl;
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			cout << digital_map[i][j] << " ";
		}
		cout << endl;
	}
	cout << endl;
}

//显示文件列表
void DisplayFileList(vector<file_info> file_list)
{
	cout << "文件列表：" << endl;
	cout << "文件名 | 占用磁盘物理块数 | 物理地址" << endl;
	for (int i = 0; i < file_list.size(); i++)
	{
		file_info file = file_list[i];
		cout << setw(4) << file.file_name << setw(14) << file.block_number;
		cout << setw(10) << " ";
		for (int j = 0; j < file.physical_blocks.size(); j++)
		{
			int cylinder_number = file.physical_blocks[j].cylinder_number;
			int track_number = file.physical_blocks[j].track_number;
			int sector_number = file.physical_blocks[j].sector_number;
			int address = cylinder_number * 8 + track_number * 4 + sector_number;
			cout << address << " ";
		}
		cout << endl;
	}
	cout << endl;
}

int main()
{
	int digital_map[8][8] = {};		//位示图
	vector<file_info> file_list;	//文件列表
	DisplayDigitalMap(digital_map);
	string input;
	getline(cin, input);
	while (input != "Q" && input != "q" && input != "")
	{
		vector<string> request_info;
		SplitString(input, request_info, " ");

		if (request_info[0] == "allocate")
		{
			bool res = Allocate(file_list, digital_map, request_info[1], stoi(request_info[2]));
			if (res)
			{
				DisplayDigitalMap(digital_map);
				DisplayFileList(file_list);
			}
			else
			{
				cout << "Allocate failed!" << endl;
			}
		}
		else if (request_info[0] == "recycle")
		{
			bool res = Recycle(file_list, digital_map, request_info[1]);
			if (res)
			{
				DisplayDigitalMap(digital_map);
				DisplayFileList(file_list);
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

	return 0;
}