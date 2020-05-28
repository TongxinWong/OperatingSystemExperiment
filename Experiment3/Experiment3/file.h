#pragma once
#include <vector>

typedef struct
{
	int cylinder_number;	//柱面号
	int track_number;		//磁道号
	int sector_number;		//物理记录号（扇区号）
}physical_address;

//文件目录表项
typedef struct
{
	std::string file_name;	//文件名
	int block_number;		//分配的物理块数
	std::vector<physical_address> physical_blocks;	//磁盘物理地址列表
}file_info;