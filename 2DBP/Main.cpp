// 2023-03-10 CG for 2D-CSP

#include "2DCG.h"
using namespace std;

int main()
{
	All_Values Values;
	All_Lists Lists;
	ReadData(Values, Lists); // ��ȡ����
	CuttingHeuristic(Values, Lists); // ��ʼ����ʽ�����һ���ʼ���и������Ϊ��ʼMP��ϵ������
	ColumnGeneration(Values, Lists);
	return 0;
}