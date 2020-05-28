#pragma once
#include <vector>

typedef struct
{
	int cylinder_number;	//�����
	int track_number;		//�ŵ���
	int sector_number;		//�����¼�ţ������ţ�
}physical_address;

//�ļ�Ŀ¼����
typedef struct
{
	std::string file_name;	//�ļ���
	int block_number;		//������������
	std::vector<physical_address> physical_blocks;	//���������ַ�б�
}file_info;