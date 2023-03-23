
#include "2DBP.h"
using namespace std;

void PrimalHeuristic(All_Values& Values, All_Lists& Lists, Node& root_node) // �ж�ʽ�и�����ʽ
{
	int item_types_num = Values.item_types_num;
	int strip_types_num = Values.strip_types_num;

	Values.Finish = false;
	Lists.occupied_items_list.clear();

	int stock_index = 0;
	int stock_pattern = 0;
	int strip_index = 0;
	int strip_pattern = 0;

#pragma region
	while (Values.Finish == false) { // ��ʼ����ĸ��

		Lists.all_stocks_list.erase(Lists.all_stocks_list.begin()); // ĸ����г�ȥĸ��0

		// Init one stock
		Stock_Stc new_stock;
		new_stock.length = Lists.all_stocks_list[0].length; // 
		new_stock.width = Lists.all_stocks_list[0].width; // 
		new_stock.area = new_stock.length * new_stock.width; // 
		new_stock.stock_idx = stock_index; // 
		new_stock.pos_x = 0; // 
		new_stock.pos_y = 0; // 

		// Init all strip_types in each stock
		for (int k = 0; k < strip_types_num; k++) {
			Strip_Type_Stc this_strip_type;
			this_strip_type.strip_type_idx = k + 1;
			new_stock.strip_types_list.push_back(this_strip_type);
		}

		// Init a new stock
		Item_Stc stock_remain;

		stock_remain.length = new_stock.length;
		stock_remain.width = new_stock.width;
		stock_remain.area = new_stock.area;
		stock_remain.pos_x = new_stock.pos_x;
		stock_remain.pos_y = new_stock.pos_y;

		stock_remain.stock_idx = new_stock.stock_idx;
		stock_remain.occupied = 0;

		int all_items_num = Lists.all_items_list.size();
		for (int j = 0; j < all_items_num; j++) {
			if (Lists.all_items_list[j].length <= stock_remain.length
				&& Lists.all_items_list[j].width <= stock_remain.width
				&& Lists.all_items_list[j].occupied == 0) { // �Ӱ� j �ܲ��ܷ��뱻��ĸ��ʣ�µ�����

				// Init first item in a this_strip, though may not use them all
				Item_Stc first_item;
				Lists.all_items_list[j].occupied = 1;

				first_item.item_type_idx = Lists.all_items_list[j].item_type_idx;
				first_item.item_idx = Lists.all_items_list[j].item_idx;
				first_item.demand = Lists.all_items_list[j].demand;

				first_item.length = Lists.all_items_list[j].length;
				first_item.width = Lists.all_items_list[j].width;
				first_item.area = first_item.length * first_item.width;
				first_item.pos_x = stock_remain.pos_x;
				first_item.pos_y = stock_remain.pos_y;

				first_item.stock_idx = stock_remain.stock_idx;
				first_item.strip_idx = strip_index;
				first_item.occupied = 1;

				Lists.occupied_items_list.push_back(first_item); // ȷ�����Ӱ��������

				// Init a this_strip acoording to its first item
				Strip_Stc new_strip;
				new_strip.strip_idx = strip_index;
				new_strip.strip_type_idx = first_item.item_type_idx;

				new_strip.length = stock_remain.length; // all this_strip's length == stock's length
				new_strip.width = first_item.width; // a strip_type is defined by its first item's width
				new_strip.area = new_strip.length * new_strip.width;
				new_strip.pos_x = first_item.pos_x;
				new_strip.pos_y = first_item.pos_y;

				new_strip.stock_idx = stock_remain.stock_idx;
				new_strip.items_in_strip_list.push_back(first_item); // a this_strip's first item defined this this_strip

				// Init all item_types in each this_strip, though may not use them all
				for (int k = 0; k < item_types_num; k++) {
					Item_Type_Stc this_item_type;
					this_item_type.item_type_idx = k + 1;
					new_strip.item_types_list.push_back(this_item_type);
				}

				int type_pos = first_item.item_type_idx - 1;
				new_strip.item_types_list[type_pos].this_item_type_num++;

				// �����ж�ʽ�и�󣬵�һ���Ӱ���Ҳ�����
				Item_Stc first_item_right_side;

				first_item_right_side.length = stock_remain.length - first_item.length; // ���򳤶� = ĸ�峤�� - ��һ���Ӱ峤��
				first_item_right_side.width = first_item.width; // ������ = ��һ���Ӱ���
				first_item_right_side.area = first_item_right_side.length * first_item_right_side.width; // �������
				first_item_right_side.pos_x = stock_remain.pos_x + first_item.length;
				first_item_right_side.pos_y = stock_remain.pos_y;

				first_item_right_side.stock_idx = stock_remain.stock_idx; // ��������ĸ��
				first_item_right_side.item_type_idx = -1;
				first_item_right_side.occupied = 0;

				// �ڵ�һ���Ӱ���Ҳ����򣬼��м���ڲ���������Ӱ�
				// ֱ����ǰ�м����Ҳ�޷������Ӱ�Ϊֹ
				for (int m = 0; m < all_items_num; m++) {
					if (Lists.all_items_list[m].length <= first_item_right_side.length
						&& Lists.all_items_list[m].width <= first_item_right_side.width
						&& Lists.all_items_list[m].occupied == 0) { // �����ǰ�Ӱ��ܹ������һ���Ӱ��Ҳ�����

						// �µ��Ӱ�����м��
						Item_Stc new_item;
						Lists.all_items_list[m].occupied = 1;

						new_item.item_type_idx = Lists.all_items_list[m].item_type_idx; // �Ӱ���
						new_item.item_idx = Lists.all_items_list[m].item_idx; // �Ӱ���
						new_item.demand = Lists.all_items_list[m].demand;

						new_item.length = Lists.all_items_list[m].length;
						new_item.width = Lists.all_items_list[m].width;
						new_item.area = new_item.length * new_item.width;
						new_item.pos_x = first_item_right_side.pos_x;
						new_item.pos_y = first_item_right_side.pos_y;

						new_item.stock_idx = stock_remain.stock_idx; // ����ĸ����
						new_item.strip_idx = strip_index;
						new_item.occupied = 1;

						Lists.occupied_items_list.push_back(new_item); // �Ӱ��������
						new_strip.items_in_strip_list.push_back(new_item);

						int itm_pos = new_item.item_type_idx - 1;
						new_strip.item_types_list[itm_pos].this_item_type_num++;

						first_item_right_side.length = first_item_right_side.length - new_item.length; // �����м���ʣ�೤��
						first_item_right_side.pos_x = first_item_right_side.pos_x + new_item.length;
					}
				}
				// ���Ѿ����ֵ������м��.��ÿ���ӹ�.��ʹ�������Ƚ�һ��

				int Strip_Final_Cnt = 0;
				int all_strips_num = Lists.all_strips_list.size();
				if (all_strips_num == 0) { // ��һ���м��
					new_strip.pattern = strip_pattern;
					strip_pattern++; // ֻ��Ψһ���и�ģʽ���Ŷ�Ӧһ��pattern
					root_node.cutting_strip_patterns_list.push_back(new_strip);
				}
				if (all_strips_num != 0) { // ��һ���м��֮�������м��
					for (int s = 0; s < all_strips_num; s++) {
						int cnt01 = 0;
						int cnt02 = 0;
						int cnt03 = 0;
						for (int k = 0; k < item_types_num; k++) { // ���������Ӱ����� 1-11
							int cnt1 = Lists.all_strips_list[s].item_types_list[k].this_item_type_num; // �����м��s���Ӱ�����k+1��ʹ�ô���
							int cnt2 = new_strip.item_types_list[k].this_item_type_num; // ���м��new_strip���Ӱ�����k+1��ʹ�ô���
							if (cnt1 == cnt2) { // ����ʹ�ô�����ͬ
								cnt01 = cnt01 + 1; // ʹ�ô�����ͬ�����+1
							}
							if (cnt1 != cnt2) { // ����ʹ�ô�����ͬ
								cnt03 = cnt03 + 1;
							}
						}
						Strip_Final_Cnt = cnt01;
						//printf("��ͬʹ�ô��� = %d\n��ͬʹ�ô��� = %d\n",cnt01,cnt03);
						if (Strip_Final_Cnt == item_types_num) { // �����Ӱ�����ʹ�ô�������ͬ
							new_strip.pattern = Lists.all_strips_list[s].pattern;
							break;
						}
					}
					// ���������м�������������ӹܵ�ʹ�ô�����
					if (Strip_Final_Cnt < item_types_num) { // ȷ�����µ��и�ģʽ
						new_strip.pattern = strip_pattern;
						strip_pattern++; // ֻ��Ψһ���и�ģʽ���Ŷ�Ӧһ��pattern
						root_node.cutting_strip_patterns_list.push_back(new_strip); // �ڶ��׶���
					}
				}

				strip_index++;
				Lists.all_strips_list.push_back(new_strip);
				new_stock.strips_list.push_back(new_strip);

				int stp_pos = new_strip.strip_type_idx - 1;
				new_stock.strip_types_list[stp_pos].this_strip_type_num++;

				// �����ж�ʽ�и�󣬵�һ���Ӱ���Ϸ�����
				stock_remain.length = stock_remain.length;
				stock_remain.width = stock_remain.width - first_item.width;
				stock_remain.area = stock_remain.length * stock_remain.width;
				stock_remain.pos_x = stock_remain.pos_x;
				stock_remain.pos_y = stock_remain.pos_y + first_item.width;

				// ʹ�õ��Ӱ�����
				int occupied_items_num = 0;
				int all_items_num = Lists.all_items_list.size();
				for (int k = 0; k < all_items_num; k++) {
					occupied_items_num += Lists.all_items_list[k].occupied;
				}
				if (occupied_items_num == all_items_num) { // ��������Ӱ嶼��ʹ����
					Values.Finish = true;
				}
			}
		}

		//********����ĸ�����и���ĳɱ�********//
		int strip_total_cut_distance = 0;
		int strips_num_in_stock = new_stock.strips_list.size();
		for (int j = 0; j < strips_num_in_stock; j++) {

			Strip_Stc this_strip = new_stock.strips_list[j];
			int item_total_cut_distance = 0;
			int items_num_in_strip = this_strip.items_in_strip_list.size();
			for (int k = 0; k < items_num_in_strip; k++) {
				Item_Stc this_item = this_strip.items_in_strip_list[k];

				if (this_item.width < this_strip.width) {
					this_item.cutting_distance = this_item.length + this_item.width;
				}
				if (this_item.width == this_strip.width) {
					this_item.cutting_distance = this_item.width;
				}

				item_total_cut_distance = item_total_cut_distance + this_item.cutting_distance;
			}

			if (this_strip.pos_x + this_strip.width < new_stock.pos_x + new_stock.width) {
				this_strip.cutting_distance = item_total_cut_distance + this_strip.length;
				this_strip.material_cutting_loss = this_strip.cutting_distance * Values.unit_cut_loss;
			}
			if (this_strip.pos_x + this_strip.width == new_stock.pos_x + new_stock.width) {
				this_strip.cutting_distance = item_total_cut_distance;
				this_strip.material_cutting_loss = this_strip.cutting_distance * Values.unit_cut_loss;
			}
			strip_total_cut_distance = strip_total_cut_distance + this_strip.cutting_distance;
		}

		new_stock.cutting_distance = strip_total_cut_distance;
		new_stock.material_cutting_loss = new_stock.cutting_distance * Values.unit_cut_loss;

		//********����ĸ��������˷ѳɱ�********//
		int strip_total_waste_area = 0;
		for (int j = 0; j < strips_num_in_stock; j++) {
			Strip_Stc this_strip = new_stock.strips_list[j];
			int item_total_used_area = 0;
			int items_num_in_strip = this_strip.items_in_strip_list.size();
			for (int k = 0; k < items_num_in_strip; k++) {
				item_total_used_area = item_total_used_area + this_strip.items_in_strip_list[k].area;
			}

			this_strip.wasted_area = this_strip.area - item_total_used_area;
			this_strip.material_area_loss = this_strip.wasted_area * Values.unit_area_loss;
			strip_total_waste_area = strip_total_waste_area + this_strip.wasted_area;
		}

		new_stock.wasted_area = new_stock.area - strip_total_waste_area;
		new_stock.material_area_loss = new_stock.wasted_area * Values.unit_area_loss;

		int Stock_Final_Cnt = 0;
		int occupied_stocks_num = Lists.occupied_stocks_list.size();
		if (occupied_stocks_num == 0) { // ��һ��ĸ��
			new_stock.pattern = stock_pattern;
			stock_pattern++; // ֻ��Ψһ���и�ģʽ���Ŷ�Ӧһ��pattern
			root_node.cutting_stock_patterns_list.push_back(new_stock);
		}

		if (occupied_stocks_num != 0) { // ��һ���м��֮������ĸ��
			for (int s = 0; s < occupied_stocks_num; s++) {
				int cnt01 = 0;
				int cnt02 = 0;
				int cnt03 = 0;
				for (int k = 0; k < strip_types_num; k++) { // ���������м������ 1-11
					int cnt1 = Lists.occupied_stocks_list[s].strip_types_list[k].this_strip_type_num; // ����ĸ��s���м������k��ʹ�ô���
					int cnt2 = new_stock.strip_types_list[k].this_strip_type_num; // ��ĸ��new_stock���м������k��ʹ�ô���

					if (cnt1 == cnt2) { // ����ʹ�ô�����ͬ
						cnt01 = cnt01 + 1; // ʹ�ô�����ͬ�����+1
					}
					if (cnt1 != cnt2) { // ����ʹ�ô�����ͬ
						cnt03 = cnt03 + 1;
					}
				}

				Stock_Final_Cnt = cnt01;
				if (Stock_Final_Cnt == strip_types_num) { // �����м������ʹ�ô�������ͬ
					break;
				}
			}

			if (Stock_Final_Cnt < strip_types_num) { // ȷ�����µ��м������
				new_stock.pattern = stock_pattern;
				stock_pattern++; // ֻ��Ψһ���и�ģʽ���Ŷ�Ӧһ��pattern
				root_node.cutting_stock_patterns_list.push_back(new_stock); // ��һ�׶���
			}
		}

		Lists.occupied_stocks_list.push_back(new_stock);
		stock_index = stock_index + 1;
	}

	/*			    pattern columns
	-----------------------------------------
	|		 K_num			|		 P_num			|
	| cut-stk-ptn cols	| stp-cut-ptn cols	|
	-----------------------------------------------------
	|							|							|				|
	|			 C				|			D				|  J_num	|	strip_type cons >= 0
	|							|							|				|
	|----------------------------------------------------
	|							|							|				|
	|			 0				|			B				|  N_num	|	item_type cons >= item_type demands
	|							|							|				|
	-----------------------------------------------------
	*/

	int K_num = root_node.cutting_stock_patterns_list.size();
	int P_num = root_node.cutting_strip_patterns_list.size();

	int J_num = strip_types_num;
	int N_num = item_types_num;

	// Init model matrix
	for (int col = 0; col < K_num + P_num; col++) {
		vector<double>temp_col;
		for (int row = 0; row < J_num + N_num; row++) {

			// Matrix C & Matrix 0
			if (col < K_num) {

				// 1. Matrix C
				if (row < J_num) {
					double temp_val =
						root_node.cutting_stock_patterns_list[col].strip_types_list[row].this_strip_type_num; // ϵ��Ϊ�м������ʹ�ô���
					temp_col.push_back(temp_val);
				}

				// 2. Matrix 0
				if (row >= J_num) {
					double temp_val = 0; //
					temp_col.push_back(temp_val);
				}
			}

			// Matrix B & Matrix D
			if (col >= K_num) {

				// 3. Matrix D
				if (row < J_num) {
					int col_pos = col - K_num;
					int item_type_idx = row + 1;
					int strip_type_idx = root_node.cutting_strip_patterns_list[col_pos].strip_type_idx;
					if (strip_type_idx == item_type_idx) // 
					{
						double temp_val = -1; // ϵ��Ϊ-1
						temp_col.push_back(temp_val);
					}
					else // �м��������Ӱ����಻��Ӧ
					{
						double temp_val = 0; // ϵ��Ϊ0
						temp_col.push_back(temp_val);
					}
				}

				// 4.Matrix B
				if (row >= J_num) {
					int col_pos = col - K_num;
					int row_pos = row - J_num;
					double temp_val =
						root_node.cutting_strip_patterns_list[col_pos].item_types_list[row_pos].this_item_type_num;
					temp_col.push_back(temp_val);
				}
			}
		}

		root_node.model_matrix.push_back(temp_col);
	}

	cout << endl;

	for (int col = 0; col < K_num; col++) {
		vector<double>temp_col;
		for (int row = 0; row < J_num + N_num; row++) {

			// 1. Matrix C
			if (row < J_num) {
				double temp_val =
					root_node.cutting_stock_patterns_list[col].strip_types_list[row].this_strip_type_num; // ϵ��Ϊ�м������ʹ�ô���
				temp_col.push_back(temp_val);
			}

			// 2. Matrix 0
			if (row >= J_num) {
				double temp_val = 0; //
				temp_col.push_back(temp_val);
			}
		}
		root_node.cutting_stock_cols.push_back(temp_col); // ��һ�׶���
	}

	cout << endl;

	for (int col = K_num; col < K_num + P_num; col++) {
		vector<double>temp_col;
		for (int row = 0; row < J_num + N_num; row++) {

			// 3. Matrix D
			if (row < J_num) {
				int col_pos = col - K_num;
				int item_type_index = row + 1;
				int strip_type_index = root_node.cutting_strip_patterns_list[col_pos].strip_type_idx;
				if (strip_type_index == item_type_index) {
					double temp_val = -1; // ϵ��Ϊ-1
					temp_col.push_back(temp_val);
				}
				else { // �м��������Ӱ����಻��Ӧ
					double temp_val = 0; // ϵ��Ϊ0
					temp_col.push_back(temp_val);
				}
			}

			// 4.Matrix B
			if (row >= J_num) {
				int col_pos = col - K_num;
				int row_pos = row - J_num;
				double temp_val =
					root_node.cutting_strip_patterns_list[col_pos].item_types_list[row_pos].this_item_type_num;
				temp_col.push_back(temp_val);
			}
		}
		root_node.cutting_strip_cols.push_back(temp_col); // �ڶ��׶���
	}

	for (int k = 0; k < item_types_num; k++) {
		Strip_Type_Stc temp_stp;
		temp_stp.width = Lists.all_item_types_list[k].width;
		temp_stp.length = Values.stock_length;

		Lists.all_strip_types_list.push_back(temp_stp);
	}
	cout << endl;
}