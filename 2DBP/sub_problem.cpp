// 2023-03-10 CG for 2D-CSP
#include "2DCG.h"
using namespace std;

int SolveOuterSubProblem(All_Values& Values, All_Lists& Lists)
{
	int stg0_cols_num = Lists.stg0_cols_list.size();
	int stg1_cols_num = Lists.stg1_cols_list.size();
	int stg2_cols_num = Lists.stg2_cols_list.size();

	int item_types_num = Values.item_types_num;
	int strip_types_num = Values.strip_types_num;

	int final_return = 0;

	IloEnv Env_OSP; // Outer SP����
	IloModel Mode_OSP(Env_OSP); // Outer SPģ��
	IloNumVarArray Vars_OSP(Env_OSP); // Outer SP���߱���

	// var >= 0
	IloNum var_min = 0; // var LB
	IloNum var_max = IloInfinity; // var UB

	for (int k = 0; k < strip_types_num; k++) // һ��һ�У������м�������Ӧ��Outer SP���߱���
	{
		IloNumVar var(Env_OSP, var_min, var_max, ILOINT); //
		Vars_OSP.add(var); // 
	}

	// Outer SP obj
	IloExpr obj_sum(Env_OSP);
	for (int k = 0; k < strip_types_num; k++) // �м������
	{
		int pos = k + item_types_num;
		obj_sum += Lists.dual_prices_list[pos] * Vars_OSP[k]; // ���� *���߱��� w_i * C_i
	}

	IloObjective Obj_OSP = IloMaximize(Env_OSP, obj_sum); // 
	Mode_OSP.add(Obj_OSP); // 

	// Outer SP cons
	IloExpr con_sum(Env_OSP);
	for (int i = 0; i < strip_types_num; i++)
	{
		con_sum += Lists.item_types_list[i].width * Vars_OSP[i]; // y_i * C_i
	}

	// con <= stock width
	Mode_OSP.add(con_sum <= Values.stock_width);
	con_sum.end();

	printf("\n///////////////////////////////// Start the CPLEX solving of Outer SP /////////////////////////////////\n");
	IloCplex Cplex_OSP(Env_OSP);
	Cplex_OSP.extract(Mode_OSP);
	Cplex_OSP.exportModel("Outer SP.lp"); // 
	bool OSP_flag = Cplex_OSP.solve(); // 
	printf("\n///////////////////////////////// Finish the CPLEX solving of Outer SP /////////////////////////////////\n\n");

	if (OSP_flag == 0)
	{
		printf("\n	Outer SP has NO feasible solution ......\n");
	}
	else
	{
		printf("\n	Outer SP has feasible solution !\n");

		printf("\n	obj_OSP = %f\n", Cplex_OSP.getValue(Obj_OSP));

		double OSP_obj_val = 0;
		vector<double> OSP_solns_list;
		vector<double> OSP_new_col; // ���Outer SP��õ�����

		printf("\n	Outer SP vars:\n");
		for (int col = 0; col < item_types_num; col++) // 
		{
			double soln_val = Cplex_OSP.getValue(Vars_OSP[col]);
			OSP_obj_val = OSP_obj_val + soln_val; // �õ�����Outer SPĿ�꺯��ֵ
			OSP_solns_list.push_back(soln_val);

			printf("\n	var_%d = %f", col+1,soln_val);
		}

		// ���Outer SP�������������
		// �����Outer SP��õ����м��뵱ǰMP���������Inner SP
		if (OSP_obj_val > 1)
		{
			printf("\n\n	Outer SP has NEGATIVE reduced cost !\n");
			printf("\n	Outer SP New Column��\n\n");

			for (int k = 0; k < item_types_num; k++) // һ��һ�У��Ӱ���
			{
				OSP_new_col.push_back(0); // ֵΪ0
				printf("	%f\n", OSP_new_col[k]);
			}
			for (int k = 0; k < strip_types_num; k++) // һ��һ�У��м����
			{
				double soln_val = Cplex_OSP.getValue(Vars_OSP[k]);
				OSP_new_col.push_back(soln_val); // ֵΪ��һ�׶ξ��߱���ֵ
				printf("	%f\n", OSP_new_col[k + strip_types_num]);
			}

			// ���ڵ�ǰ��һ�׶��еĺ���
			Lists.stg0_cols_list.insert(Lists.stg0_cols_list.begin() + Lists.stg1_cols_list.size(), OSP_new_col); // �µ�һ�׶��м���������vector
			Lists.stg1_cols_list.push_back(OSP_new_col); // �µ�һ�׶��м����һ�׶���vector
			Lists.new_cols_list.push_back(OSP_new_col); // �µ�һ�׶��м��뵱ǰ����vector

			OSP_new_col.clear();

			final_return = 0;
		}

		// ���Outer SPδ�������������
		// �������������м���Ӧ��Inner SP�����ܷ��������
		if (OSP_obj_val <= 1)
		{
			printf("\n	Outer SP has POSITIVE reduced cost ......\n");
			printf("\n	Procceed to solve Inner SP\n");

			// ��ǰOuter SP��Ӧ��Inner SP
			// ����Ƕ�������⣬�ڲ�
			int OSP_solns_num = Vars_OSP.getSize(); // ѭ���Ĵ�����ÿ����һ�׶��ж���Ӧһ��Inner SP���������Ӱ���������
			for (int OSP_iter = 0; OSP_iter < OSP_solns_num; OSP_iter++) // 
			{
				double soln_val = OSP_solns_list[OSP_iter];
				final_return = SolveInnerSubProblem(Values, Lists, soln_val, OSP_iter);
			}
		}
	}




	return final_return; // �������յķ���ֵ
}

int SolveInnerSubProblem(All_Values& Values, All_Lists& Lists, double OSP_soln_val, int OSP_iter)
{
	int final_return = -1;

	int item_types_num = Values.item_types_num;
	int strip_types_num = Values.strip_types_num;

	IloEnv Env_ISP; // Inner SP����
	IloModel Model_ISP(Env_ISP); // Inner SPģ��
	IloNumVarArray Vars_ISP(Env_ISP); // Inner SP

	IloNum  var_min = 0; // Inner SP���߱����½�=0
	IloNum  var_max = IloInfinity; // Inner SP���߱����Ͻ�=������

	for (int k = 0; k < item_types_num; k++) // һ���ӹ����࣬��Ӧһ��Inner SP���߱���
	{
		IloNumVar var(Env_ISP, var_min, var_max, ILOINT); // Inner SP���߱���������
		Vars_ISP.add(var); // Inner SP���߱�������list
	}

	// ����Inner SP��Ŀ�꺯��
	IloExpr obj_sum(Env_ISP);
	for (int k = 0; k < item_types_num; k++)  // һ���ӹ����࣬��Ӧһ��Inner SP���߱���
	{
		obj_sum += Lists.dual_prices_list[k] * Vars_ISP[k]; // ���ӣ���ż�۸�*���߱���
	}
	IloObjective Obj_ISP = IloMaximize(Env_ISP, obj_sum); // ����Ŀ�꺯���������ֵ��
	Model_ISP.add(Obj_ISP); // Ŀ�꺯��add()��ģ��

	// ����Inner SP��Լ��
	IloExpr con_sum(Env_ISP);
	for (int k = 0; k < item_types_num; k++) // һ���ӹ����࣬��Ӧһ��Inner SP���߱���
	{
		con_sum += Lists.item_types_list[k].length * Vars_ISP[k];
	}

	// һ���м���и����Ӱ峤��֮�ͱ���С�ڵ�ǰ�м�壬Ҳ���ǵ�ǰĸ��ĳ���
	Model_ISP.add(con_sum <= Values.stock_length);
	con_sum.end();

	printf("\n///////////////////////////////// Start the CPLEX solving of SP2_%d /////////////////////////////////\n", OSP_iter + 1);
	IloCplex Cplex_ISP(Env_ISP);
	Cplex_ISP.extract(Model_ISP);
	Cplex_ISP.exportModel("Sub Problem 2.lp"); // ���Inner SP��lpģ��
	Cplex_ISP.solve(); // ���Inner SP
	printf("\n///////////////////////////////// Finish the CPLEX solving of SP2_%d /////////////////////////////////\n\n", OSP_iter + 1);


	printf("\n	Inner SP vars:\n");
	vector<double> ISP_solns_list;
	double ISP_obj_val = 0;

	for (int k = 0; k < item_types_num; k++)
	{
		double soln_val = Cplex_ISP.getValue(Vars_ISP[k]);
		ISP_obj_val = ISP_obj_val + soln_val; // ������յľ��߱���ֵ
		ISP_solns_list.push_back(soln_val);

		printf("	var_%d = %f\n",k+1, soln_val);
	}

	vector<double> ISP_new_col; // ���Inner SP��õ�����
	if (ISP_obj_val > OSP_soln_val) // Inner SP��Ŀ�꺯��ֵ > ��Ӧ��Outer SP���߱���ֵ
	{
		printf("\n	Inner SP has NEGATIVE reduced cost !\n");

		// һ��һ��ȷ��Inner SP����
		for (int ISP_iter = 0; ISP_iter < item_types_num; ISP_iter++) // һ��һ�У��Ӱ���
		{
			double soln_val = Cplex_ISP.getValue(Vars_ISP[ISP_iter]);
			ISP_new_col.push_back(soln_val); // ֵΪInner SP���߱���ֵ
		}

		for (int ISP_iter = 0; ISP_iter < strip_types_num; ISP_iter++) //  һ��һ�У��м����
		{
			if (OSP_iter == ISP_iter) // �Ӱ�������м���Ӧ
			{
				ISP_new_col.push_back(-1); // ��Ӧ�ϵĻ���ֵ��Ϊ-1
			}

			if (OSP_iter != ISP_iter) // �Ӱ�������м�岻��Ӧ 
			{
				ISP_new_col.push_back(0); // û��Ӧ�ϵĻ���ֵ��Ϊ0
			}
		}

		printf("\n	SP2 %d New Column:\n", OSP_iter + 1);
		for (int k = 0; k < item_types_num + strip_types_num; k++) // ���Inner SP������
		{
			printf("	%f\n", ISP_new_col[k]);
		}
		final_return = 0; // ���Inner SP�õ��������У�����0
	}

	// ���Inner SPû�������������
	if (ISP_obj_val <= OSP_soln_val) // Inner SPĿ�꺯��С�ڶ�Ӧ��Outer SP���߱���ֵ
	{
		printf("\n	Inner SP has POSITIVE reduced cost......\n");

		final_return = 1; // ���Inner SPû�еõ��������У�����1�������㷨������
	}

	// Inner SP��õ����У��ŵ�����ϵ���е����
	Lists.stg0_cols_list.insert(Lists.stg0_cols_list.end(), ISP_new_col); // Inner SP���м��������е�vector��������ĩβ
	Lists.stg2_cols_list.push_back(ISP_new_col); // Inner SP���м���ڶ��׶��е�vector
	Lists.new_cols_list.push_back(ISP_new_col); // Inner SP���м��뵱ǰ��������vector

	ISP_new_col.clear(); // ���Inner SP����

	return 0;
}
