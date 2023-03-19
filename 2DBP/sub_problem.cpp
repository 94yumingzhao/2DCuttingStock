// Yuming Zhao: https://github.com/Baturuym
// 2023-03-10 CG for 2D-CSP

#include "2DCG.h"
using namespace std;

int SolveOuterSubProblem(All_Values& Values, All_Lists& Lists)
{
	int all_cols_num = Lists.model_matrix.size();

	int strip_types_num = Values.strip_types_num;
	int item_types_num = Values.item_types_num;

	int P = Lists.strip_cols.size();
	int K = Lists.item_cols.size();

	int J = strip_types_num;
	int N = item_types_num;

	int final_return = 0;

	IloEnv Env_OSP; // Outer-SP����
	IloModel Model_OSP(Env_OSP); // Outer-SPģ��
	IloNumVarArray Vars_Ga(Env_OSP); // Outer-SP���߱���


	for (int k = 0; k < J; k++) // 
	{
		// var >= 0
		IloNum var_min = 0; // var LB
		IloNum var_max = IloInfinity; // var UB
		string Ga_name = "G_" + to_string(k + 1);

		IloNumVar var_ga(Env_OSP, var_min, var_max, ILOINT, Ga_name.c_str()); //
		Vars_Ga.add(var_ga); // 
	}

	// Outer-SP obj
	IloExpr obj_sum(Env_OSP);
	for (int k = 0; k < J; k++) // vars num = strip_types num
	{
		double a_val = Lists.dual_prices_list[k];
		obj_sum += a_val * Vars_Ga[k]; //  obj: sum (a_j * G_j)
	}

	IloObjective Obj_OSP = IloMaximize(Env_OSP, obj_sum);
	Model_OSP.add(Obj_OSP); // 
	obj_sum.end();

	// Outer-SP only one con
	IloExpr con_sum(Env_OSP);
	for (int k = 0; k < J; k++)
	{
		double ws_val = Lists.strip_types_list[k].width;
		con_sum += ws_val * Vars_Ga[k]; // con: sum (ws_j * G_j) <= W
	}

	Model_OSP.add(con_sum <= Values.stock_width);
	con_sum.end();

	printf("\n///////////////////////////////// OSP_%d CPLEX solving START /////////////////////////////////\n\n", Values.iter);
	IloCplex Cplex_OSP(Env_OSP);
	Cplex_OSP.extract(Model_OSP);
	Cplex_OSP.exportModel("Outer Sub Problem.lp"); // 
	bool OSP_flag = Cplex_OSP.solve(); // 
	printf("\n///////////////////////////////// OSP_%d CPLEX solving OVER /////////////////////////////////\n\n", Values.iter);

	if (OSP_flag == 0)
	{
		printf("\n	Outer-SP NOT FEASIBLE\n");

		Obj_OSP.removeAllProperties();
		Obj_OSP.end();
		Vars_Ga.clear();
		Vars_Ga.end();
		Model_OSP.removeAllProperties();
		Model_OSP.end();
		Env_OSP.removeAllProperties();
		Env_OSP.end();
	}
	else
	{
		printf("\n	Outer-SP FEASIBLE\n");

		printf("\n	OBJ = %f\n", Cplex_OSP.getValue(Obj_OSP));

		double OSP_obj_val = Cplex_OSP.getValue(Obj_OSP);

		// print OSP solns
		Lists.new_strip_col.clear();
		printf("\n	Outer-SP Vars:\n\n");
		for (int k = 0; k < J; k++) // strip rows
		{
			double soln_val = Cplex_OSP.getValue(Vars_Ga[k]);
			printf("	var_G_%d = %f\n", k + 1, soln_val);
			Lists.new_strip_col.push_back(soln_val); // 
		}

		// print OSP new col
		printf("\n	Outer-SP_%d new col:\n\n", Values.iter);
		for (int k = 0; k < J; k++)
		{
			double soln_val = Cplex_OSP.getValue(Vars_Ga[k]);
			printf("	row_%d = %f\n", k + 1, soln_val);
		}
		for (int k = J; k < J+N; k++) // item rows
		{
			printf("	row_%d = 0\n", k + 1);
			Lists.new_strip_col.push_back(0);
		}

		Obj_OSP.removeAllProperties();
		Obj_OSP.end();
		Vars_Ga.clear();
		Vars_Ga.end();
		Model_OSP.removeAllProperties();
		Model_OSP.end();
		Env_OSP.removeAllProperties();
		Env_OSP.end();

		if (OSP_obj_val > 1) // �����Outer-SP��õ����м��뵱ǰMP���������Inner SP
		{
			printf("\n\n	Outer-SP reduced cost = %f > 1,  \n", OSP_obj_val);
			printf("\n	No need to solve Inner-SP\n");

			final_return = 0;
		}

		if (OSP_obj_val <= 1) 	// �������������м���Ӧ��Inner SP�����ܷ��������
		{
			printf("\n	Outer-SP reduced cost = %f <=1 \n", OSP_obj_val);
			printf("\n	Continue to solve Inner SP\n");

			SolveInnerSubProblem(Values, Lists);

			Values.ISP_obj_val = -1;
			Lists.new_item_cols.clear();
			Lists.ISP_new_col.clear();
			
			for (int p = 0; p < J; p++) // ÿ����һ�׶��ж���Ӧһ��Inner SP
			{
				double soln_val = Lists.new_strip_col[p];
				if (soln_val > 0) // only a feasible stk cutting pattern deserve its stp cutting pattern
				{
					if (Values.ISP_obj_val > soln_val)
					{
						vector<double> temp_col; // ���Inner SP��õ�����
						for (int k = 0; k < J; k++) //  һ��һ�У��м����
						{
							if (p == k) // �Ӱ�������м���Ӧ
							{
								temp_col.push_back(-1); // ��Ӧ�ϵĻ���ֵ��Ϊ-1
							}

							if (p != k) // �Ӱ�������м�岻��Ӧ 
							{
								temp_col.push_back(0); // û��Ӧ�ϵĻ���ֵ��Ϊ0
							}
						}

						for (int k = J; k < J+N; k++) // һ��һ�У��Ӱ���
						{
							double soln_val = Lists.ISP_new_col[k];
							temp_col.push_back(soln_val); // ֵΪInner SP���߱���ֵ
						}

						printf("\n	ISP new col:\n\n");
						for (int k = 0; k < J + N; k++) // ���Inner SP������
						{
							printf("	row_%d = %f\n", k + 1, temp_col[k]);
						}

						Lists.new_item_cols.push_back(temp_col);
						printf("\n	Add OSP_%d ISP new col to MP\n\n", Values.iter);
					}
					else
					{
						printf("\n	OSP_%d has no ISP new col \n\n", Values.iter);
					}
				}
			}
			
		}
	}

	return final_return; // �������յķ���ֵ
}

int SolveInnerSubProblem(All_Values& Values, All_Lists& Lists)
{
	int all_cols_num = Lists.model_matrix.size();

	int strip_types_num = Values.strip_types_num;
	int item_types_num = Values.item_types_num;

	int P = Lists.strip_cols.size();
	int K = Lists.item_cols.size();

	int J = strip_types_num;
	int N = item_types_num;

	int final_return = -1;

	IloEnv Env_ISP; // Inner SP����
	IloModel Model_ISP(Env_ISP); // Inner SPģ��
	IloNumVarArray Vars_De(Env_ISP); // Inner SP

	for (int k = 0; k < N; k++) // һ���ӹ����࣬��Ӧһ��Inner SP���߱���
	{
		IloNum  var_min = 0; // 
		IloNum  var_max = IloInfinity; // 
		string De_name = "D_" + to_string(k + 1);

		IloNumVar var_de(Env_ISP, var_min, var_max, ILOINT, De_name.c_str()); // Inner SP���߱���������
		Vars_De.add(var_de); // Inner SP���߱�������list
	}

	// Inner-SP's obj
	IloExpr obj_sum(Env_ISP);
	for (int k = 0; k < N; k++)  // һ���ӹ����࣬��Ӧһ��Inner SP���߱���
	{
		int row_pos = k + N;
		double b_val = Lists.dual_prices_list[row_pos];
		obj_sum += b_val * Vars_De[k]; // ���ӣ���ż�۸�*���߱���
	}

	IloObjective Obj_ISP = IloMaximize(Env_ISP, obj_sum); //
	Model_ISP.add(Obj_ISP); //
	obj_sum.end();

	// Inner-SP's only one con
	IloExpr con_sum(Env_ISP);
	for (int k = 0; k < N; k++) // һ���ӹ����࣬��Ӧһ��Inner SP���߱���
	{
		double li_val = Lists.item_types_list[k].length;
		con_sum += li_val * Vars_De[k];
	}

	Model_ISP.add(con_sum <= Values.stock_length);
	con_sum.end();

	printf("\n///////////////////////////////// OSP_%d ISP CPLEX solving START /////////////////////////////////\n", Values.iter);
	IloCplex Cplex_ISP(Env_ISP);
	Cplex_ISP.extract(Model_ISP);
	Cplex_ISP.exportModel("Inner Sub Problem.lp"); // ���Inner SP��lpģ��
	bool ISP_flag = Cplex_ISP.solve(); // ���Inner SP
	printf("\n///////////////////////////////// OSP_%d ISP CPLEX solving OVER /////////////////////////////////\n\n", Values.iter);

	if (ISP_flag == 0)
	{
		printf("\n	Inner-SP NOT FEASIBLE\n");
	}
	else
	{
		printf("\n	Inner-SP FEASIBLE\n");

		printf("\n	OBJ = %f\n", Cplex_ISP.getValue(Obj_ISP));
		Values.ISP_obj_val = Cplex_ISP.getValue(Obj_ISP);


		for (int k = 0; k < J; k++)
		{
			Lists.ISP_new_col.push_back(-1);
		}

		printf("\n	Inner SP vars:\n\n");
		for (int k = 0; k < N; k++)
		{
			double soln_val = Cplex_ISP.getValue(Vars_De[k]);
			printf("	var_D_%d = %f\n", k + 1, soln_val);
			Lists.ISP_new_col.push_back(soln_val);
		}
		
		final_return = 0; // ���Inner SP�õ��������У�����0
	}

	Obj_ISP.removeAllProperties();
	Obj_ISP.end();
	Vars_De.clear();
	Vars_De.end();
	Model_ISP.removeAllProperties();
	Model_ISP.end();
	Env_ISP.removeAllProperties();
	Env_ISP.end();

	return 0;
}
