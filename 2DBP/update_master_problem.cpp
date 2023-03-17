// 2023-03-10 CG for 2D-CSP
#include "2DCG.h"
using namespace std;

void SolveUpdateMasterProblem(
	All_Values& Values, 
	All_Lists& Lists,
	IloEnv& Env_MP, 
	IloModel& Model_MP, 
	IloObjective& Obj_MP,
	IloRangeArray& Cons_MP, 
	IloNumVarArray& Vars_MP)
{
	int item_types_num = Values.item_types_num;
	int strip_types_num = Values.strip_types_num;
	int all_rows_num = item_types_num + strip_types_num;

	// �����Outer SP��Inner SP���ɵ�����
	int new_cols_num = Lists.new_cols_list.size();
	for (int col = 0; col < new_cols_num; col++) // 
	{
		IloNum obj_para = 1; // ���ж�Ӧ��Ŀ�꺯���о��߱�����ϵ����Ϊ1
		IloNumColumn CplexCol = (Obj_MP)(obj_para); // 

		for (int row = 0; row < all_rows_num; row++)
		{
			IloNum row_para = Lists.new_cols_list[col][row];
			CplexCol += (Cons_MP)[row](row_para); //
		}

		IloNum var_min = 0; // var LB
		IloNum var_max = IloInfinity;  // var UB

		IloNumVar var(CplexCol, var_min, var_max, ILOFLOAT); // �������ɳڱ���
		(Vars_MP).add(var);

		CplexCol.end();
	}
	

	printf("\n\n---------Start the CPLEX solving of the NEW MP---------\n");
	IloCplex MP_cplex(Model_MP);
	MP_cplex.extract(Model_MP);
	MP_cplex.exportModel(" New MP.lp"); // �����ǰMP��ģ�� 
	MP_cplex.solve(); // ��⵱ǰMP
	printf("---------Finish the CPLEX solving of the NEW MP---------\n\n");

	Lists.dual_prices_list.clear();
	for (int row = 0; row < item_types_num + strip_types_num; row++) // һ��һ�У����е��Ӱ���
	{
		double dual_val = MP_cplex.getDual(Cons_MP[row]);
		Lists.dual_prices_list.push_back(dual_val); // ��һ��Լ��getDual()���ż��
	}

	Lists.new_cols_list.clear(); // ����������е�vector

	printf("The objective function is %f\n\n", MP_cplex.getValue(Obj_MP));
	printf("\n");
}

void SolveFinalMasterProblem(
	All_Values& Values,
	All_Lists& Lists,
	IloEnv& Env_MP,
	IloModel& Model_MP,
	IloObjective& Obj_MP,
	IloRangeArray& Cons_MP,
	IloNumVarArray& Vars_MP)
{
	
	printf("\n\n---------Start the CPLEX solving of the NEW MP---------\n");
	IloCplex MP_cplex(Model_MP);
	MP_cplex.extract(Model_MP);
	MP_cplex.exportModel(" New MP.lp"); // �����ǰMP��ģ�� 
	MP_cplex.solve(); // ��⵱ǰMP
	printf("---------Finish the CPLEX solving of the NEW MP---------\n\n");


	printf("The objective function is %f\n\n", MP_cplex.getValue(Obj_MP));
	printf("\n");
}