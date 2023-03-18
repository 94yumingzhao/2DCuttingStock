// Yuming Zhao: https://github.com/Baturuym
// 2023-03-10 CG for 2D-CSP

#include "2DCG.h"
using namespace std;

void InitModelMatrix(All_Values& Values, All_Lists& Lists) // �ж�ʽ�и�����ʽ
{
	int item_types_num = Values.item_types_num;
	int strip_types_num = Values.strip_types_num;

	Values.Finish = false;
	Lists.items_occupied_list.clear();

	int stock_index = 0;
	int stock_pattern = 0;
	int strip_index = 0;
	int strip_pattern = 0;

#pragma region
	while (Values.Finish == false)
	{
		// ��ʼ����ĸ��
		Lists.stock_pool_list.erase(Lists.stock_pool_list.begin()); // ĸ����г�ȥĸ��0

		StockProperties new_stock;
		new_stock.length = Lists.stock_pool_list[0].length; // ĸ��
		new_stock.width = Lists.stock_pool_list[0].width; // ĸ��
		new_stock.area = new_stock.length * new_stock.width; // ĸ��
		new_stock.index = stock_index; // ĸ��
		new_stock.x_value = 0; // ĸ��
		new_stock.y_value = 0; // ĸ��

		for (int k = 0; k < strip_types_num; k++)
		{
			StripTypeProperties this_strip_type;
			this_strip_type.type = k + 1;
			new_stock.strip_types_list.push_back(this_strip_type);
		}

		// ÿ��һ����ĸ�壬�ͳ�ʼ����ĸ��ʣ���
		ItemProperties stock_remain;
		stock_remain.length = new_stock.length;
		stock_remain.width = new_stock.width;
		stock_remain.type = -1;
		stock_remain.area = new_stock.area;
		stock_remain.stock_index = new_stock.index;
		stock_remain.x_value = new_stock.x_value;
		stock_remain.y_value = new_stock.y_value;
		stock_remain.occupied = 0;

		int all_items_list_size1 = Lists.all_items_list.size();
		for (int j = 0; j < all_items_list_size1; j++)
		{
			// �Ӱ� j �ܲ��ܷ��뱻��ĸ��ʣ�µ�����
			if (Lists.all_items_list[j].length <= stock_remain.length
				&& Lists.all_items_list[j].width <= stock_remain.width
				&& Lists.all_items_list[j].occupied == 0)
			{
				// ȷ����һ���Ӱ�
				ItemProperties first_item;
				first_item.type = Lists.all_items_list[j].type;
				first_item.index = Lists.all_items_list[j].index;
				first_item.demand = Lists.all_items_list[j].demand;
				first_item.length = Lists.all_items_list[j].length;
				first_item.width = Lists.all_items_list[j].width;
				first_item.area = first_item.length * first_item.width;
				first_item.stock_index = stock_remain.stock_index;
				first_item.Strip_index = strip_index;
				first_item.x_value = stock_remain.x_value;
				first_item.y_value = stock_remain.y_value;
				first_item.occupied = 1;
				Lists.all_items_list[j].occupied = 1;
				Lists.items_occupied_list.push_back(first_item); // ȷ�����Ӱ��������

				StripProperties new_strip;
				new_strip.index = strip_index;
				new_strip.type = first_item.type;
				new_strip.items_list.push_back(first_item); // ��ʼ����һ��Ԫ��
				new_strip.length = stock_remain.length;
				new_strip.width = first_item.width;
				new_strip.area = new_strip.length * new_strip.width;
				new_strip.x_value = first_item.x_value;
				new_strip.y_value = first_item.y_value;
				new_strip.stock_index = stock_remain.stock_index;

				for (int k = 0; k < item_types_num; k++)
				{
					ItemTypeProperties this_item_type;
					this_item_type.type = k + 1;
					new_strip.item_types_list.push_back(this_item_type);
				}
				new_strip.item_types_list[first_item.type - 1].count++;

				// �����ж�ʽ�и�󣬵�һ���Ӱ���Ҳ�����
				ItemProperties first_item_right_side;
				first_item_right_side.length = stock_remain.length - first_item.length; // ���򳤶� = ĸ�峤�� - ��һ���Ӱ峤��
				first_item_right_side.width = first_item.width; // ������ = ��һ���Ӱ���
				first_item_right_side.area = first_item_right_side.length * first_item_right_side.width; // �������
				first_item_right_side.stock_index = stock_remain.stock_index; // ��������ĸ��
				first_item_right_side.type = -1;
				first_item_right_side.x_value = stock_remain.x_value + first_item.length;
				first_item_right_side.y_value = stock_remain.y_value;
				first_item_right_side.occupied = 0;

				// �ڵ�һ���Ӱ���Ҳ����򣬼��м���ڲ���������Ӱ�
				// ֱ����ǰ�м����Ҳ�޷������Ӱ�Ϊֹ
				int all_items_list_size2 = Lists.all_items_list.size();
				for (int m = 0; m < all_items_list_size2; m++)
				{
					// �����ǰ�Ӱ��ܹ������һ���Ӱ��Ҳ�����
					if (Lists.all_items_list[m].length <= first_item_right_side.length
						&& Lists.all_items_list[m].width <= first_item_right_side.width
						&& Lists.all_items_list[m].occupied == 0)
					{
						// �µ��Ӱ�����м��
						ItemProperties new_item;
						new_item.type = Lists.all_items_list[m].type; // �Ӱ���
						new_item.index = Lists.all_items_list[m].index; // �Ӱ���
						new_item.demand = Lists.all_items_list[m].demand;
						new_item.length = Lists.all_items_list[m].length;
						new_item.width = Lists.all_items_list[m].width;
						new_item.area = new_item.length * new_item.width;
						new_item.x_value = first_item_right_side.x_value;
						new_item.y_value = first_item_right_side.y_value;
						new_item.stock_index = stock_remain.stock_index; // ����ĸ����
						new_item.Strip_index = strip_index;
						new_item.occupied = 1;
						Lists.all_items_list[m].occupied = 1;

						Lists.items_occupied_list.push_back(new_item); // �Ӱ��������

						new_strip.items_list.push_back(new_item);
						new_strip.item_types_list[new_item.type - 1].count++;

						first_item_right_side.length = first_item_right_side.length - new_item.length; // �����м���ʣ�೤��
						first_item_right_side.x_value = first_item_right_side.x_value + new_item.length;
					}
				}
				// ���Ѿ����ֵ������м��.��ÿ���ӹ�.��ʹ�������Ƚ�һ��

				int Strip_Final_Cnt = 0;
				if (Lists.all_strips_list.size() == 0) // ��һ���м��
				{
					new_strip.pattern = strip_pattern;
					strip_pattern++; // ֻ��Ψһ���и�ģʽ���Ŷ�Ӧһ��pattern
					Lists.item_col_ptns_list.push_back(new_strip);
				}
				if (Lists.all_strips_list.size() != 0) // ��һ���м��֮�������м��
				{
					int all_strips_list_size = Lists.all_strips_list.size();
					for (int s = 0; s < all_strips_list_size; s++)
					{
						//printf("���м��%d���м��%d���Ա�\n", new_strip.index,s);
						int cnt01 = 0;
						int cnt02 = 0;
						int cnt03 = 0;
						for (int k = 0; k < item_types_num; k++) // ���������Ӱ����� 1-11
						{
							int cnt1 = Lists.all_strips_list[s].item_types_list[k].count; // �����м��s���Ӱ�����k+1��ʹ�ô���
							int cnt2 = new_strip.item_types_list[k].count; // ���м��new_strip���Ӱ�����k+1��ʹ�ô���

							if (cnt1 == cnt2) // ����ʹ�ô�����ͬ
							{
								cnt01 = cnt01 + 1; // ʹ�ô�����ͬ�����+1
							}
							if (cnt1 != cnt2) // ����ʹ�ô�����ͬ
							{
								cnt03 = cnt03 + 1;
							}
						}
						Strip_Final_Cnt = cnt01;
						//printf("��ͬʹ�ô��� = %d\n��ͬʹ�ô��� = %d\n",cnt01,cnt03);
						if (Strip_Final_Cnt == item_types_num) // �����Ӱ�����ʹ�ô�������ͬ
						{
							new_strip.pattern = Lists.all_strips_list[s].pattern;
							break;
						}
					}
					// ���������м�������������ӹܵ�ʹ�ô�����
					if (Strip_Final_Cnt < item_types_num) // ȷ�����µ��и�ģʽ
					{
						new_strip.pattern = strip_pattern;
						strip_pattern++; // ֻ��Ψһ���и�ģʽ���Ŷ�Ӧһ��pattern
						Lists.item_col_ptns_list.push_back(new_strip); // �ڶ��׶���
					}
				}
				Lists.all_strips_list.push_back(new_strip);
				strip_index++;
				new_stock.strips_list.push_back(new_strip);
				new_stock.strip_types_list[new_strip.type - 1].count++;

				// �����ж�ʽ�и�󣬵�һ���Ӱ���Ϸ�����
				stock_remain.length = stock_remain.length;
				stock_remain.width = stock_remain.width - first_item.width;
				stock_remain.area = stock_remain.length * stock_remain.width;
				stock_remain.x_value = stock_remain.x_value;
				stock_remain.y_value = stock_remain.y_value + first_item.width;

				// ʹ�õ��Ӱ�����
				int Used_sum = 0;
				int all_items_list_size3 = Lists.all_items_list.size();
				for (int k = 0; k < all_items_list_size3; k++)
				{
					Used_sum += Lists.all_items_list[k].occupied;
				}
				// ��������Ӱ嶼��ʹ����
				if (Used_sum == all_items_list_size3)
				{
					Values.Finish = true;
				}
			}
		}
		//********����ĸ�����и���ĳɱ�********//
		int strip_total_cut_distane = 0;
		int new_stock_strips_list_size1 = new_stock.strips_list.size();
		for (int j = 0; j < new_stock_strips_list_size1; j++)
		{
			StripProperties strip = new_stock.strips_list[j];
			int item_total_cut_distance = 0;
			int strip_items_list_size = strip.items_list.size();
			for (int k = 0; k < strip_items_list_size; k++)
			{
				ItemProperties item = strip.items_list[k];
				if (item.width < strip.width)
				{
					item.cutting_distance = item.length + item.width;
				}
				if (item.width == strip.width)
				{
					item.cutting_distance = item.width;
				}
				item_total_cut_distance = item_total_cut_distance + item.cutting_distance;
			}
			if (strip.x_value + strip.width < new_stock.x_value + new_stock.width)
			{
				strip.cutting_distance = item_total_cut_distance + strip.length;
				strip.material_cutting_loss = strip.cutting_distance * Values.unit_cut_loss;
			}
			if (strip.x_value + strip.width == new_stock.x_value + new_stock.width)
			{
				strip.cutting_distance = item_total_cut_distance;
				strip.material_cutting_loss = strip.cutting_distance * Values.unit_cut_loss;
			}
			strip_total_cut_distane = strip_total_cut_distane + strip.cutting_distance;
		}
		new_stock.cutting_distance = strip_total_cut_distane;
		new_stock.material_cutting_loss = new_stock.cutting_distance * Values.unit_cut_loss;

		//********����ĸ��������˷ѳɱ�********//
		int strip_total_waste_area = 0;
		int new_stock_strips_list_size2 = new_stock.strips_list.size();
		for (int j = 0; j < new_stock_strips_list_size2; j++)
		{
			StripProperties strip = new_stock.strips_list[j];
			int item_total_used_area = 0;
			int strip_items_list_size = strip.items_list.size();
			for (int k = 0; k < strip_items_list_size; k++)
			{
				item_total_used_area = item_total_used_area + strip.items_list[k].area;
			}
			strip.wasted_area = strip.area - item_total_used_area;
			strip.material_area_loss = strip.wasted_area * Values.unit_area_loss;
			strip_total_waste_area = strip_total_waste_area + strip.wasted_area;
		}
		new_stock.wasted_area = new_stock.area - strip_total_waste_area;
		new_stock.material_area_loss = new_stock.wasted_area * Values.unit_area_loss;

		int Stock_Final_Cnt = 0;
		if (Lists.occupied_stocks_list.size() == 0) // ��һ��ĸ��
		{
			new_stock.pattern = stock_pattern;
			stock_pattern++; // ֻ��Ψһ���и�ģʽ���Ŷ�Ӧһ��pattern
			Lists.strip_col_ptns_list.push_back(new_stock);
		}

		if (Lists.occupied_stocks_list.size() != 0) // ��һ���м��֮������ĸ��
		{
			for (int s = 0; s < Lists.occupied_stocks_list.size(); s++)
			{
				int cnt01 = 0;
				int cnt02 = 0;
				int cnt03 = 0;

				for (int k = 0; k < strip_types_num; k++) // ���������м������ 1-11
				{
					int cnt1 = Lists.occupied_stocks_list[s].strip_types_list[k].count; // ����ĸ��s���м������k��ʹ�ô���
					int cnt2 = new_stock.strip_types_list[k].count; // ��ĸ��new_stock���м������k��ʹ�ô���

					if (cnt1 == cnt2) // ����ʹ�ô�����ͬ
					{
						cnt01 = cnt01 + 1; // ʹ�ô�����ͬ�����+1
					}
					if (cnt1 != cnt2) // ����ʹ�ô�����ͬ
					{
						cnt03 = cnt03 + 1;
					}
				}

				Stock_Final_Cnt = cnt01;
				if (Stock_Final_Cnt == strip_types_num) // �����м������ʹ�ô�������ͬ
				{
					break;
				}
			}

			if (Stock_Final_Cnt < strip_types_num) // ȷ�����µ��м������
			{
				new_stock.pattern = stock_pattern;
				stock_pattern++; // ֻ��Ψһ���и�ģʽ���Ŷ�Ӧһ��pattern
				Lists.strip_col_ptns_list.push_back(new_stock); // ��һ�׶���
			}
		}
		Lists.occupied_stocks_list.push_back(new_stock);
		stock_index = stock_index + 1;
	}

	int P = Lists.strip_col_ptns_list.size();
	int K = Lists.item_col_ptns_list.size();

	int J = strip_types_num;
	int N = item_types_num;

	/*
				P						K
	 strip_types cols  item_types cols
	----------------------------------
	|						|						|
	|			C		    |           D			|	J-strip_types rows >= 0
	|						|						|
	|---------------------------------
	|						|						|
	|			0			|			B			|	N-item_types rows >= item_type demand
	|						|						|
	----------------------------------

	*/

	cout << endl;

	// Init model matrix
	for (int col = 0; col < P + K; col++)
	{
		vector<double>temp_col;
		for (int row = 0; row < J + N; row++)
		{
			// Matrix C & Matrix 0
			if (col < P)
			{
				// 1. Matrix C
				if (row < J)
				{
					double temp_val =
						Lists.strip_col_ptns_list[col].strip_types_list[row].count; // ϵ��Ϊ�м������ʹ�ô���
					temp_col.push_back(temp_val);
				}
				// 2. Matrix 0
				if (row >= J)
				{
					double temp_val = 0; //
					temp_col.push_back(temp_val);
				}
			}
			// Matrix B & Matrix D
			if (col >= P)
			{
				// 3. Matrix D
				if (row < J)
				{
					int col_pos = col - P;
					int item_type_idx = row + 1;
					if (Lists.item_col_ptns_list[col_pos].type == item_type_idx) // 
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
				if(row>=J)
				{
					int col_pos = col - P;
					int row_pos = row - J;
					double temp_val = Lists.item_col_ptns_list[col_pos].item_types_list[row_pos].count;
					temp_col.push_back(temp_val);
				}
			}
		}
		Lists.model_matrix.push_back(temp_col);
	}

	cout << endl;

	for (int col = 0; col < P; col++)
	{
		vector<double>temp_col;
		for (int row = 0; row < J + N; row++)
		{
			// 1. Matrix C
			if (row < J)
			{
				double temp_val = Lists.strip_col_ptns_list[col].strip_types_list[row].count; // ϵ��Ϊ�м������ʹ�ô���
				temp_col.push_back(temp_val);
			}
			// 2. Matrix 0
			if (row >= J)
			{
				double temp_val = 0; //
				temp_col.push_back(temp_val);
			}
		}
		Lists.strip_cols_list.push_back(temp_col); // ��һ�׶���
	}

	cout << endl;

	for (int col = P; col < P+K; col++)
	{
		vector<double>temp_col;
		for (int row = 0; row < J + N; row++)
		{
			// 3. Matrix D
			if (row < J)
			{
				int col_pos = col - P;
				int item_type_idx = row + 1;
				if (Lists.item_col_ptns_list[col_pos].type == item_type_idx) // 
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
			if (row >= J)
			{
				int col_pos = col - P;
				int row_pos = row - J;
				double temp_val = Lists.item_col_ptns_list[col_pos].item_types_list[row_pos].count;
				temp_col.push_back(temp_val);
			}
		}
		Lists.item_cols_list.push_back(temp_col); // �ڶ��׶���
	}
	cout << endl;
}