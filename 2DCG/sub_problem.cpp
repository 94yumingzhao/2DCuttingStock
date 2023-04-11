﻿
// 2023-03-10 CG for 2D-CSP

#include "2DCG.h"
using namespace std;

int SolveWidthSubProblem(All_Values& Values, All_Lists& Lists) {

	int K_num = Lists.cutting_stock_cols.size();
	int P_num = Lists.cutting_strip_cols.size();

	int J_num = Values.strip_types_num;
	int N_num = Values.item_types_num;

	int all_cols_num = K_num + P_num;
	int all_rows_num = J_num + N_num;

	/*			    pattern columns
	-----------------------------------------
	|		 P_num			|		K_num			|
	| cut-stk-ptn cols		| cut-stp-ptn cols	|
	-----------------------------------------------------
	|						|							|				|
	|			 C				|			D				|  J_num	|	strip_type cons >= 0
	|							|							|				|
	|----------------------------------------------------
	|							|							|				|
	|			 0				|			B				|  N_num	|	item_type cons >= item_type demand
	|							|							|				|
	-----------------------------------------------------
	*/

	int loop_continue_flag = -1;

	IloEnv Env_WSP; // WSP环境
	IloModel Model_WSP(Env_WSP); // WSP模型
	IloNumVarArray Vars_Ga(Env_WSP); // WSP决策变量

	for (int j = 0; j < J_num; j++) {
		IloNum var_min = 0; // var LB
		IloNum var_max = IloInfinity; // var UB, var >= 0
		string var_name = "G_" + to_string(j + 1);
		IloNumVar Var_Ga(Env_WSP, var_min, var_max, ILOINT, var_name.c_str()); //
		Vars_Ga.add(Var_Ga); // 
	}

	// WSP obj
	IloExpr obj_sum(Env_WSP);
	for (int j = 0; j < J_num; j++) { // vars num = strip_types num
		double temp_val = Lists.dual_prices_list[j];
		obj_sum += temp_val * Vars_Ga[j]; //  obj: sum (a_j * G_j)
	}
	IloObjective Obj_WSP = IloMaximize(Env_WSP, obj_sum);
	Model_WSP.add(Obj_WSP); // 
	obj_sum.end();

	// WSP only one con
	IloExpr con_sum(Env_WSP);
	for (int j = 0; j < J_num; j++) {
		double temp_val = Lists.all_strip_types_list[j].width;
		con_sum += temp_val * Vars_Ga[j]; // con: sum (ws_j * G_j) <= W
	}
	Model_WSP.add(con_sum <= Values.stock_width);
	con_sum.end();

	printf("\n///////////////// WSP_%d CPLEX solving START /////////////////\n\n", Values.iter);
	IloCplex Cplex_WSP(Env_WSP);
	Cplex_WSP.extract(Model_WSP);
	Cplex_WSP.exportModel("Outer Sub Problem.lp"); // 
	bool WSP_flag = Cplex_WSP.solve(); // 
	printf("\n///////////////// WSP_%d CPLEX solving OVER /////////////////\n\n", Values.iter);

	if (WSP_flag == 0) {
		printf("\n\t WSP_%d is NOT FEASIBLE\n", Values.iter);
	}
	else {
		printf("\n\t WSP_%d is FEASIBLE\n", Values.iter);
		printf("\n\t Obj = %f\n", Cplex_WSP.getValue(Obj_WSP));

		double WSP_obj_val = Cplex_WSP.getValue(Obj_WSP);
		vector<double> WSP_solns_list;

		// print WSP solns
		Lists.new_cutting_stock_col.clear();
		printf("\n\t WSP_%d VARS:\n\n", Values.iter);
		for (int j = 0; j < J_num; j++) { // this_strip rows
			double soln_val = Cplex_WSP.getValue(Vars_Ga[j]);
			if (soln_val == -0) {
				soln_val = 0;
			}
			printf("\t var_G_%d = %f\n", j + 1, soln_val);
			Lists.new_cutting_stock_col.push_back(soln_val);
			WSP_solns_list.push_back(soln_val);
		}

		// print WSP new col
		printf("\n\t WSP_%d new col:\n\n", Values.iter);
		for (int j = 0; j < J_num + N_num; j++) {
			if (j < J_num) {
				double soln_val = Cplex_WSP.getValue(Vars_Ga[j]);
				if (soln_val == -0) {
					soln_val = 0;
				}
				printf("\t row_%d = %f\n", j + 1, soln_val);
			}
			else {
				printf("\t row_%d = 0\n", j + 1);
				Lists.new_cutting_stock_col.push_back(0);
			}
		}

		if (WSP_obj_val > 1 + RC_EPS) { // 则求解WSP获得的新列加入当前MP，不用求解LSP
			printf("\n\n\t WSP reduced cost = %f > 1,  \n", WSP_obj_val);
			printf("\n\t No need to solve Inner-SP\n");

			loop_continue_flag = 1;
		}
		else { // 则继续求解这张中间板对应的LSP，看能否求出新列
			printf("\n\t WSP reduced cost = %f <=1 \n", WSP_obj_val);
			printf("\n\t Continue to solve LSP\n");

			Values.LSP_obj_val = -1;
			Lists.new_cutting_strip_cols.clear();

			Lists.LSP_solns_list.clear();
			Lists.LSP_new_col.clear();

			SolveLengthSubProblem(Values, Lists);

			int feasible_flag = 0;

			int K_num = Lists.cutting_stock_cols.size();
			int P_num = Lists.cutting_strip_cols.size();

			for (int k = 0; k < J_num; k++) { // all current stk-cut-patterns
				double a_val = Lists.dual_prices_list[k];
				//SolveLengthSubProblem(Values, Lists);

				if (Values.LSP_obj_val > a_val + RC_EPS) {
					feasible_flag = 1;

					printf("\n\t WSP_%d_LSP_%d reduced cost = %f > strip_type con_%d dual = %f:\n",
						Values.iter, k + 1, Values.LSP_obj_val, k + 1, a_val);

					vector<double> temp_col; // 
					for (int j = 0; j < J_num; j++) { //  all current stp-cut-patterns 
						if (k == j) {   // this stp-cut-pattern p is USED in stk-cut-pattern k
							temp_col.push_back(-1); // used
						}
						else {
							temp_col.push_back(0); // not used
						}
					}

					for (int i = 0; i < N_num; i++) {
						double D_soln_val = Lists.LSP_solns_list[i];
						if (D_soln_val == -0) {
							D_soln_val = 0;
						}
						temp_col.push_back(D_soln_val); // 
					}

					printf("\n\t WSP_%d_LSP_%d new col:\n\n", Values.iter, k + 1);
					for (int row = 0; row < J_num + N_num; row++) {
						printf("\t row_%d = %f\n", row + 1, temp_col[row]);  // 输出LSP的新列
					}
					printf("\n\t Add WSP_%d_LSP_%d new col to MP\n\n", Values.iter, k + 1);

					Lists.new_cutting_strip_cols.push_back(temp_col);

					cout << endl;
				}
				else {
					printf("\n\t WSP_%d_LSP_%d reduced cost = %f < strip_type con_%d dual = %f:\n",
						Values.iter, k + 1, Values.LSP_obj_val, k + 1, a_val);
					cout << endl;
				}
			}

			if (feasible_flag == 0) {
				printf("\n\t WSP_%d_LSP has no new col \n\n", Values.iter);
				printf("\n\t Column Generation loop break\n");
				cout << endl;
			}

			loop_continue_flag = feasible_flag;
		}
	}

	Obj_WSP.removeAllProperties();
	Obj_WSP.end();
	Vars_Ga.clear();
	Vars_Ga.end();
	Model_WSP.removeAllProperties();
	Model_WSP.end();
	Env_WSP.removeAllProperties();
	Env_WSP.end();

	return loop_continue_flag; // 函数最终的返回值
}

void SolveLengthSubProblem(All_Values& Values, All_Lists& Lists) {
	int all_cols_num = Lists.model_matrix.size();

	int strip_types_num = Values.strip_types_num;
	int item_types_num = Values.item_types_num;

	int J_num = strip_types_num;
	int N_num = item_types_num;

	int final_return = -1;

	IloEnv Env_LSP; // LSP环境
	IloModel Model_LSP(Env_LSP); // LSP模型
	IloNumVarArray Vars_De(Env_LSP); // LSP

	for (int i = 0; i < N_num; i++) // 
	{
		IloNum  var_min = 0; // 
		IloNum  var_max = IloInfinity; // 
		string De_name = "D_" + to_string(i + 1);

		IloNumVar Var_De(Env_LSP, var_min, var_max, ILOINT, De_name.c_str()); // LSP决策变量，整数
		Vars_De.add(Var_De); // LSP决策变量加入list
	}

	// Inner-SP's obj
	IloExpr obj_sum(Env_LSP);
	for (int i = 0; i < N_num; i++) {
		int row_pos = i + N_num;
		double b_val = Lists.dual_prices_list[row_pos];
		obj_sum += b_val * Vars_De[i]; // 连加：对偶价格*决策变量
	}
	IloObjective Obj_LSP = IloMaximize(Env_LSP, obj_sum); //
	Model_LSP.add(Obj_LSP); //
	obj_sum.end();

	// Inner-SP's only one con
	IloExpr con_sum(Env_LSP);
	for (int i = 0; i < N_num; i++) {
		double li_val = Lists.all_item_types_list[i].length;
		con_sum += li_val * Vars_De[i];
	}

	Model_LSP.add(con_sum <= Values.stock_length);
	con_sum.end();

	printf("\n///////////////// WSP_%d_LSP CPLEX solving START /////////////////\n\n", Values.iter);
	IloCplex Cplex_LSP(Env_LSP);
	Cplex_LSP.extract(Model_LSP);
	Cplex_LSP.exportModel("Inner Sub Problem.lp"); // 输出LSP的lp模型
	bool LSP_flag = Cplex_LSP.solve(); // 求解LSP
	printf("\n///////////////// WSP_%d_LSP CPLEX solving OVER /////////////////\n\n", Values.iter);

	if (LSP_flag == 0) {
		printf("\n\t WSP_%d_LSP is NOT FEASIBLE\n", Values.iter);
	}
	else {
		printf("\n\t WSP_%d_LSP is FEASIBLE\n", Values.iter);

		printf("\n\t Obj = %f\n", Cplex_LSP.getValue(Obj_LSP));
		Values.LSP_obj_val = Cplex_LSP.getValue(Obj_LSP);

		for (int j = 0; j < J_num; j++) {
			Lists.LSP_new_col.push_back(-1);
		}

		printf("\n\t WSP_%d_LSP VARS:\n\n", Values.iter);
		for (int i = 0; i < N_num; i++) {
			double soln_val = Cplex_LSP.getValue(Vars_De[i]);
			if (soln_val == -0) {
				soln_val = 0;
			}
			printf("\t var_D_%d = %f\n", i + 1, soln_val);
			Lists.LSP_new_col.push_back(soln_val);
			Lists.LSP_solns_list.push_back(soln_val);
		}
	}

	Obj_LSP.removeAllProperties();
	Obj_LSP.end();
	Vars_De.clear();
	Vars_De.end();
	Model_LSP.removeAllProperties();
	Model_LSP.end();
	Env_LSP.removeAllProperties();
	Env_LSP.end();
}
