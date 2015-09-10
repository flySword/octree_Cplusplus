#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <vector>
#include <algorithm>
#include "culPoint.h"

using namespace std;


int xyz2morton(int x, int y, int z)
{
	int temp = 0;
	for(int i=0; i<9; i++)
	{
		temp |= ((x & (1<<i))<<i*2) | ((y & (1<<i))<<(i*2+1)) | ((z & (1<<i))<<(i*2+2)) ;
	}
	//cout<<temp<<endl;
	return temp;
}

void morton2xyz(int morton, int &x, int &y, int &z)
{
	x = 0;
	y = 0; 
	z = 0;
	for(int i=0; i<9; i++)
	{
		if((morton & (1<<i*3)) != 0 )
			x |= 1<<i;
		if((morton & (1<<(i*3+1))) != 0 )
			y |= 1<<i;
		if((morton & (1<<(i*3+2))) != 0)
			z |= 1<<i;
	}

}


struct octreeNode{
	float data;
	int morton;
	bool operator < (const octreeNode &m)const {
		return morton < m.morton;
	}
};
bool createOctree(string filename,string outFilename)
{
	ifstream fileRead(filename);
	ofstream fileOut(outFilename);
	if(fileRead == NULL)
	{
		cout<<"file read failed"<<endl;
		return false;
	}	
	
	// դ��������Ԫ��Ŀ
	int xCount;
	int yCount;
	int zCount;

	// �˲������½�����
	float xOrigin;
	float yOrigin;
	float zOrigin;

	// �˲�����Ԫ��С
	float xSize;
	float ySize;
	float zSize;
	
	fileRead>>xCount>>yCount>>zCount;
	fileRead>>xOrigin>>yOrigin>>zOrigin;
	fileRead>>xSize>>ySize>>zSize;

	
	int octreeSize = xCount*yCount*zCount;	//�����ļ��İ˲�����Ԫ����
	vector<octreeNode> nodes(octreeSize);//�����ļ��е�����������Ӧ��morton��

//	vector<float> data(xCount*yCount*zCount);
//	data.reserve(xCount*yCount*zCount);


	//float *data = new float[xCount*yCount*zCount];
	
	//�˲����߳�����  �������ã�		TODO
	int OctreeEdgeLength;	
	if(xCount>yCount)
		OctreeEdgeLength = xCount;
	else
		OctreeEdgeLength = yCount;
	if(zCount > OctreeEdgeLength)
		OctreeEdgeLength = zCount;


	//morton��  ʹ��5+9*3λ���� ����+zyx
//	int *morton = new int[octreeSize];
	//int mortonTemp = 0;
	
	for(int i=0; i<zCount; i++)
	{
		for(int j=0; j<yCount; j++)
		{
			for(int k=0; k<xCount; k++)
			{
				//fileRead>>data[xyz2morton(k,j,i)];
				fileRead>>nodes[i*xCount*yCount + j*xCount + k].data;
				if(nodes[i*xCount*yCount + j*xCount + k].data == 0)
				{
					cout<<nodes[i*xCount*yCount + j*xCount + k].morton<<endl;
				}
				nodes[i*xCount*yCount + j*xCount + k].morton = xyz2morton(k,j,i);
			}
		}
	}

	//��morton���������
	sort(nodes.begin(),nodes.end());

	//��morton����кϲ�   ������һ��
	vector<octreeNode> nodesCompressed(octreeSize);
	int count = 0;
	int num111 = 1 | 1<<1 | 1<<2;
	bool isMerge; 
	for(int i=0; i<octreeSize; i++)
	{
		if((nodes[i].morton & num111)  == 0)	//ȡmorton��ĺ���λ
		{
			isMerge = true;
			for(int j=1; j<8; j++)
			{
				//���morton�������Ҳ������涨��Χ
				if(nodes[i].morton != nodes[i+j].morton - j   ||   abs(nodes[i].data-nodes[i+j].data) > 0.1)
				{
					isMerge = false;
					break;
				}
			}

			nodesCompressed[count].morton = nodes[i].morton;
			nodesCompressed[count].data   = nodes[i].data;
			count++;
			if(isMerge == true)		//	������Ժϲ�			TODO
			{
				i = i+8;
			}							
		}
		else	//	�������߼����⣡��
		{
			nodesCompressed[count].morton = nodes[i].morton;
			nodesCompressed[count].data   = nodes[i].data;
			count++;
		}	
	}
	nodesCompressed[count].morton = -999;	//������ĩβ���б��
//	cout<<count<<endl;

	fileOut<<xOrigin<<" "<<yOrigin<<" "<<zOrigin<<endl;
	fileOut<<xSize<<" "<<ySize<<" "<<zSize<<endl;
	fileOut<<xCount<<" "<<yCount<<" "<<zCount<<endl;
	fileOut<<count<<endl;
	fileOut<<1<<endl;
	fileOut<<"float"<<endl;


	for(vector<octreeNode>::iterator i=nodesCompressed.begin(); i != nodesCompressed.end(); i++)
	{
		if(i->morton == -999)
			break;	
		fileOut<<i->morton<<endl;
	}
	for(vector<octreeNode>::iterator i=nodesCompressed.begin(); i != nodesCompressed.end(); i++)
	{
		if(i->morton == -999)
			break;	
		fileOut<<i->data<<endl;
	}
	return true;
}

//��ȡ�˲����е�morton�벢����
bool readOctree(string filename,vector<int> &mortons)
{
	ifstream fileRead(filename);
	if(fileRead == NULL)
	{
		cout<<"file read failed"<<endl;
		return false;
	}	

	// �˲������½�����
	float xOrigin;
	float yOrigin;
	float zOrigin;

	// �˲�����Ԫ��С
	float xSize;
	float ySize;
	float zSize;

	// դ��������Ԫ��Ŀ
	int xCount;
	int yCount;
	int zCount;

	fileRead>>xOrigin>>yOrigin>>zOrigin;
	fileRead>>xSize>>ySize>>zSize;
	fileRead>>xCount>>yCount>>zCount;

	int count;
	int dataTypeNum;
	string dataType;
	fileRead>>count;
	fileRead>>dataTypeNum;
	fileRead>>dataType;

	mortons.resize(count);
	//mortons = new vector<int>(count);
	for(int i=0; i<count; i++)
	{
		fileRead>>mortons[i];
	}
	return true;
}

/// TODO  sdlfj
//todo:ע������  TODO
//todo:��δ����				!!!!!!!!!!!!!!!!!!!!�����б����
int findMorton(const vector<int> &mortons,int morton,int initIndex)
{
	int index = initIndex;
	int tm = mortons[index]-morton;	//���������Ŀ��Ī����֮��ľ���
	int tm2 = index;	//���index��������ѭ��

	while(mortons[index] != morton)
	{
		tm = mortons[index]-morton;
		if(tm == 1 || tm == -1)
		{
			index -= tm;
			if((mortons[index] - morton) * tm < 0)
				return -999;
		}
		else
		{
			tm2 = index;
			index -= (int)(tm*0.9);	//0.9���ڿ��Ե���  TODO
			if(index<0)
				index = 0;
			if(index > mortons.size())
				index = mortons.size()-1;
			//���mortons[index]����������������������
			while((mortons[index] - morton)*tm < 0)	
			{				
				if(tm<10)
					index += tm>0?1:-1;
				else
					index += (int)(tm*0.1);
			}
			if(tm2 == index)
				return -999;
		}

		//while(mortons[index] < tm)
		//if(abs(mortons[index] - morton) > tm )
		//{
		//	abs(mortons[index] - morton)< 10
		//	a = (mortons[index]-morton?1:-1);
		//	while(morton[index] != morton)
		//		index -= a;
		//}
	}
	return index;
}


int main()
{
	//createOctree("3d02.txt","octreeOut.txt");
	//int x,y,z;

	//morton2xyz(xyz2morton(2,1,1),x,y,z);
	//cout<<x<<"  "<<y<<"  "<<z<<endl;
	
	//ֱ�ߴ�������������������
	//Line line(5,10,0,0,-1,1);
	//Point point;
	//culPoint(line,point, 0,-20,0,20,20,20);
	//return 0;

	vector<int> mortons;
	readOctree("octreeOut.txt",mortons);
	cout<<mortons[10]<<endl;
	
	for(int i=0; i<500; i++)
		cout<<i<<"  "<<findMorton(mortons, i, 1000)<<endl;

}