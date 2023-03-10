// 2023-03-10 BP for 2D-CSP
#include "2DBP.h"
using namespace std;

void SolveUpdateMasterProblem(
	All_Values& Values, All_Lists& Lists,
	IloEnv& Env_MP, IloModel& Model_MP, IloObjective& Obj_MP,
	IloRangeArray& Cons_MP, IloNumVarArray& Vars_MP, int Final_Solve)
{
	int stg0_cols_num = Lists.stg0_cols_list.size();
	int stg1_cols_num = Lists.stg1_cols_list.size();
	int stg2_cols_num = Lists.stg2_cols_list.size();
	int item_types_num = Values.item_types_num;
	int strip_types_num = Values.strip_types_num;


	// �����Outer SP��Inner SP���ɵ�����
	int new_cols_num = Lists.new_cols_list.size();
	for (int col = 0; col < new_cols_num; col++) // һ��һ�У�Outer SP��Outer SP���ɵ���������
	{
		int Obj_Coeff = 1; // ���ж�Ӧ��Ŀ�꺯���о��߱�����ϵ����Ϊ1
		IloNumColumn CplexCol = (Obj_MP)(Obj_Coeff); // ����һ���ж���

		for (int row = 0; row < item_types_num + strip_types_num; row++)
		{
			CplexCol += (Cons_MP)[row](Lists.new_cols_list[col][row]); // ��ֵ
		}

		float var_min = 0; // MP���߱����½�
		float var_max = IloInfinity;  // MP���߱����Ͻ�

		// ��������һ�����MP
		if (Final_Solve == 0)
		{
			IloNumVar var(CplexCol, var_min, var_max, ILOFLOAT); // �������ɳڱ���
			(Vars_MP).add(var);
		}
		// ����������һ�����MP
		if (Final_Solve == 1)
		{
			IloNumVar var(CplexCol, var_min, var_max, ILOINT); // �������ɳڱ��� 
			(Vars_MP).add(var);
		}
		CplexCol.end();
	}
	Lists.new_cols_list.clear(); // ����������е�vector

	printf("\n\n---------Start the CPLEX solving of the NEW MP---------\n");
	IloCplex Cplex_MP(Model_MP);
	Cplex_MP.extract(Model_MP);
	Cplex_MP.exportModel(" New MP.lp"); // �����ǰMP��ģ�� 
	Cplex_MP.solve(); // ��⵱ǰMP
	printf("---------Finish the CPLEX solving of the NEW MP---------\n\n");

	Lists.dual_prices_list.clear();
	for (int row = 0; row < item_types_num + strip_types_num; row++) // һ��һ�У����е��Ӱ���
	{
		Lists.dual_prices_list[row] = Cplex_MP.getDual((Cons_MP)[row]); // ��һ��Լ��getDual()���ż��
	}

	printf("The objective function is %f\n\n", Cplex_MP.getValue(Obj_MP));
	printf("\n");
}