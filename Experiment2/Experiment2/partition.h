#pragma once

//���з���
typedef struct node
{
	int id;					//������
	node* next_partition;	//ָ�������һ��ĵ�ַ
	int start_address;
	int length;
}partition;

//�ѷ���ķ���
typedef struct allocated_node
{
	int pid;
	allocated_node* next_partition;	//ָ�������һ���ַ
	int start_address;
	int length;
}allocated_partition;