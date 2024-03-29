
// 2023-03-15

#include "2DBP.h"
using namespace std;

int BranchAndPriceTree(All_Values& Values, All_Lists& Lists) {

	Values.node_num = 1; // Root Node already generated

	while (1) {
		if (Values.search_flag == 0) { // search_flag set to branch current Parent Node

			Node parent_node;
			int parent_branch_flag = ChooseNodeToBranch(Values, Lists, parent_node); // decide the Node to branch

			if (parent_branch_flag == 0) {
				printf("\n\t Branch and Bound SOLVING OVER!!\n");
				printf("\n\t Final Optimal Lower Bound = %f\n\n\n", Values.optimal_LB);
				break;
			}

			if (parent_branch_flag == 1) {

				Node new_left_node;
				Node new_right_node;

				// ATTENTION: always start from the Left Node
				Values.branch_status = 1;
				Values.node_num++;
				GenerateNewNode(Values, Lists, new_left_node, parent_node); // set the Left Node
				NewNodeColumnGeneration(Values, Lists, new_left_node, parent_node); // solve the Left Node with CG loop
				int left_search_flag = FinishNode(Values, Lists, new_left_node); // finish the Left Node
				Lists.all_nodes_list.push_back(new_left_node);

				// Then the Right Node
				Values.branch_status = 2;
				Values.node_num++;
				GenerateNewNode(Values, Lists, new_right_node, parent_node); // set the Right Node
				NewNodeColumnGeneration(Values, Lists, new_right_node, parent_node); // solve the Right Node with CG loop
				int right_search_flag = FinishNode(Values, Lists, new_right_node); // finish the Right Node
				Lists.all_nodes_list.push_back(new_right_node);

				Values.root_flag = 0; // ATTENTION: 

				// ATTETION: the var-to-branch val of the Parent Node decide which Node to fathom in next while-iter
				double parent_branch_val = parent_node.var_to_branch_soln;
				if (parent_branch_val <= 1) {
					Values.search_flag = right_search_flag;

					if (Values.search_flag != 1) { // fathom the Right Nodeand branch it in next while - iter
						Values.fathom_flag = 2;
						printf("\n\t parent branch val = %.4f < 1, \n\n\t Have to fathom Right Node_%d\n",
							parent_branch_val, new_right_node.index);
					}
				}
				if (parent_branch_val > 1) { // choose the Node with better LB to fathom
					if (new_left_node.LB < new_right_node.LB) {
						Values.search_flag = left_search_flag;
						
						if (Values.search_flag != 1) { // continue to fathom the Left Node and branch it in next while-iter
							Values.fathom_flag = 1;
							printf("\n\t Left Node_%d LB %.4f < Right Node_%d LB %.4f\n",
								new_left_node.index, new_left_node.LB,
								new_right_node.index, new_right_node.LB);
							printf("\n\t continue to fathom RIGHT Node_%d\n", new_right_node.index);
						}
					}
					if (new_left_node.LB >= new_right_node.LB) {
						Values.search_flag = right_search_flag;
						
						if (Values.search_flag != 1) { // continue to set to fathom the Right Node and branch it in next while-iter
							Values.fathom_flag = 2;
							printf("\n\t Left Node_%d LB %.4f >= Right Node_%d LB %.4f\n",
								new_left_node.index, new_left_node.LB,
								new_right_node.index, new_right_node.LB);
							printf("\n\t continue to fathom RIGHT Node_%d\n", new_right_node.index);
						}
					}
				}
				Values.branch_status = 1; // ATTENTION: branch_status set to the Left Node in next while-iter
			}
		}

		if (Values.search_flag == 1) { // continue to choose a previously generated Node
			Values.branch_status = 3; // ATTENTION: branch_status set to find a previously generated Node in Tree
			Values.fathom_flag = -1; // ATTENTION: better deactivate fathom_flag
			Values.search_flag = 0; // ATTENTION: search_flag set to continue to the next while-iter
			printf("\n\t Solns of this Node are all INTEGERS! \n");
			printf("\n\t Current Optimal Lower Bound = %f\n", Values.optimal_LB);
		}

		if (Values.node_num > 30) {
			printf("\n	//////////// PROCEDURE STOP 3 //////////////\n");
			break;
		}

		// continue while-iter
	}

	return 0;
}