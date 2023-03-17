// 2023-03-10 CG for 2D-CSP

#include "2DCG.h"
using namespace std;

void ColumnGeneration(All_Values& Values, All_Lists& Lists)
{
	IloEnv Env_MP; // MP����
	IloModel Model_MP(Env_MP); // MPģ��
	IloNumVarArray Vars_MP(Env_MP); // MP���о��߱���
	IloRangeArray Cons_MP(Env_MP); // MP����Լ��
	IloObjective Obj_MP = IloAdd(Model_MP, IloMinimize(Env_MP)); // MPĿ�꺯��

	SolveFirstMasterProblem(Values, Lists, Env_MP, Model_MP, Obj_MP, Cons_MP, Vars_MP); // ����ʼMP

	while (1)
	{
		int SP_flag;

		SP_flag = SolveOuterSubProblem(Values, Lists); // ���������

		// ������SP1��SP2��û�еõ���������
		if (SP_flag == 1)
		{
			cout << endl;
			break;
		}

		// ������SP1�õ��������У�
		// �������SP1û�еõ��������У��������SP2�õ���
		if (SP_flag == 0)
		{
			SolveUpdateMasterProblem(
				Values,
				Lists,
				Env_MP,
				Model_MP,
				Obj_MP,
				Cons_MP,
				Vars_MP); // ����������SP�����еĸ���MP
		}
	}

	SolveFinalMasterProblem(
		Values,
		Lists,
		Env_MP,
		Model_MP,
		Obj_MP,
		Cons_MP,
		Vars_MP); // ���һ�����MP���õ�����������

	Obj_MP.removeAllProperties();
	Obj_MP.end();
	Vars_MP.clear();
	Vars_MP.end();
	Cons_MP.clear();
	Cons_MP.end();
	Model_MP.removeAllProperties();
	Model_MP.end();
	Env_MP.removeAllProperties();
	Env_MP.end();
}