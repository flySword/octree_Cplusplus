#pragma once

struct Point
{
	Point(float x, float y, float z)
	{
		xyz[0] = x;
		xyz[1] = y;
		xyz[2] = z;
	}
	Point()
	{
		Point(-999,-999,-999);
	}
	float xyz[3];
};

//ֱ�߷���: (x-x0)/i = (y-y0)/j = (z-z0)/k
struct Line
{
	Line(float x, float y, float z, float i, float j, float k)
	{
		xyz[0] = x;
		xyz[1] = y;
		xyz[2] = z;
		ijk[0] = i;
		ijk[1] = j;
		ijk[2] = k;
	}
	float xyz[3];
	float ijk[3];
};

//��ֱ������������Ԫ�Ľ���
//�ֱ����6������ֱ�ߵ��������㣬Ȼ���ų�һ�����㣬��ʣ�µĽ���ͨ��point����
bool culPoint(const Line &line, Point &point, float cellX, float cellY, float cellZ, float cellLength, float cellWidth, float cellHeight)
{
	//�贫���xyz����Ϊ��Ԫ����С������
	//��line�е�xyzΪ���������

	float maxX = cellX + cellLength;
	float maxY = cellY + cellWidth;
	float maxZ = cellZ + cellHeight;

	float surface[6];
	surface[0] = cellX;//�൱����Ԫ�е�x��Сֵ
	surface[1] = cellY;
	surface[2] = cellZ;
	surface[3] = maxX;
	surface[4] = maxY;
	surface[5] = maxZ;

	Point points[2];//���ڴ�ֱ�������������������
	char resultNum = 0;

	float tm;
	float tm1;
	float tm2;

	char i1;
	char i2;
	char i3;
	for(int i=0; i<6; i++)//ÿ��ͨ��һ������������������������
	{		

		i1 = i%3;	//��0��3������ֱ���д���x��ֵ��y��z
		//��1��4������ֱ���д���y����
		i2 = (i1+1)%3;
		i3 = (i1+2)%3;

		if(line.ijk[i1] == 0)
			continue;
		tm = (surface[i]-line.xyz[i1])/line.ijk[i1];
		tm1 = tm*line.ijk[i2] + line.xyz[i2];
		tm2 = tm*line.ijk[i3] + line.xyz[i3];

		if(tm1<surface[i2+3] && tm1>surface[i2] && tm2<surface[i3+3] && tm2>surface[i3])
		{
			if(i1 == 0)
			{
				points[resultNum].xyz[0] = surface[i];
				points[resultNum].xyz[1] = tm1;
				points[resultNum].xyz[2] = tm2;
			}
			if(i1 == 1)
			{
				points[resultNum].xyz[0] = tm2;
				points[resultNum].xyz[1] = surface[i];
				points[resultNum].xyz[2] = tm1;
			}
			if(i1 == 2)
			{
				points[resultNum].xyz[0] = tm1;
				points[resultNum].xyz[1] = tm2;
				points[resultNum].xyz[2] = surface[i];
			}
			resultNum++;
			if(resultNum == 2)
				break;
		}	
	}
	//test
	//for(int i=0; i<3; i++)
	//	cout<<points[0].xyz[i]<<endl;

	//for(int i=0; i<3; i++)
	//	cout<<points[1].xyz[i]<<endl;

	if(resultNum != 2)
		return false;
	else
		return true;
}