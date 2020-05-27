#pragma once

//空闲分区
typedef struct node
{
	int id;					//分区号
	node* next_partition;	//指向表中下一项的地址
	int start_address;
	int length;
}partition;

//已分配的分区
typedef struct allocated_node
{
	int pid;
	allocated_node* next_partition;	//指向表中下一项地址
	int start_address;
	int length;
}allocated_partition;