// Yuming Zhao: https://github.com/Baturuym
// 2023-03-10 CG for 2D-CSP

#include "2DCG.h"
using namespace std;

void SplitString(const string& s, vector<string>& v, const string& c)
{
	string::size_type pos1, pos2;
	pos2 = s.find(c);
	pos1 = 0;
	v.clear();//ɾ��ԭ����
	while (string::npos != pos2)
	{
		v.push_back(s.substr(pos1, pos2 - pos1));
		pos1 = pos2 + c.size();
		pos2 = s.find(c, pos1);
	}
	if (pos1 != s.length())
		v.push_back(s.substr(pos1));
}

void ReadData(All_Values& Values, All_Lists& Lists) // ����ʽ��ȡ����
{
	ostringstream s_in, s_out;//IO�ļ�
	string in_str, out_str;//IO�ļ���
	ofstream f_out;//����ļ�
	string line;//�����ݵ�һ��
	vector<string> data_inline, data_inline1, data_inline2;//��ȡ�������е�����

	s_in.str("");
	s_in << "C:/Users/YMZhao/Desktop/2DBP/2DBP/cutdata1207.txt";
	//s_in << "D:/CuttingTXT/cutdata11251.txt";
	in_str = s_in.str();

	ifstream fin(in_str.c_str());

	if (fin)
	{
		getline(fin, line);
		SplitString(line, data_inline, "\t"); // ��1��
		Values.stocks_num = atoi(data_inline[0].c_str()); // ��1�е�1λ��ĸ������

		getline(fin, line);
		SplitString(line, data_inline, "\t"); // ��2��
		Values.item_types_num = atoi(data_inline[0].c_str()); // ��2�е�1λ���Ӱ���������
		Values.strip_types_num = Values.item_types_num;

		getline(fin, line);
		SplitString(line, data_inline, "\t"); // ��3+�� ĸ����

		Values.stock_length = atoi(data_inline[0].c_str()); // ĸ�峤��
		Values.stock_width = atoi(data_inline[1].c_str()); // ĸ����

		for (int i = 0; i < Values.stocks_num; i++)
		{
			StockProperties this_stock;
			this_stock.length = Values.stock_length;
			this_stock.width = Values.stock_width;
			this_stock.area = Values.stock_length * Values.stock_width;
			this_stock.x_value = 0;
			this_stock.y_value = 0;
			Lists.stock_pool_list.insert(Lists.stock_pool_list.begin(), this_stock);
		}

		int item_index = 1;
		int item_types_num = Values.item_types_num;

		for (int i = 0; i < item_types_num; i++) // �����Ӱ���
		{
			getline(fin, line);
			SplitString(line, data_inline, "\t");

			int item_demand = atoi(data_inline[2].c_str());
			for (int k = 0; k < item_demand; k++) // �Ӱ�������
			{
				ItemProperties this_item;
				this_item.type = atoi(data_inline[3].c_str()); // �Ӱ��е�4λ���Ӱ�����
				this_item.index = item_index; // �Ӱ���ţ���1��ʼ
				this_item.demand = atoi(data_inline[2].c_str()); // �Ӱ��е�3λ���Ӱ�����		
				this_item.length = atoi(data_inline[0].c_str()); // �Ӱ��е�1λ���Ӱ峤��
				this_item.width = atoi(data_inline[1].c_str()); // �Ӱ��е�2λ���Ӱ���
				this_item.area = this_item.length * this_item.width; // �Ӱ����
				this_item.x_value = -1; // �Ӱ����Ͻ�x_value����
				this_item.y_value = -1; // �Ӱ����Ͻ�y_value����
				this_item.stock_index = -1; // �Ӱ�����ĸ����
				this_item.occupied = 0;
				Lists.all_items_list.push_back(this_item);

				item_index++;
				Values.items_num++;
			}

			ItemTypeProperties this_item_type;
			this_item_type.type = atoi(data_inline[3].c_str());
			this_item_type.demand = atoi(data_inline[2].c_str());
			this_item_type.width = atoi(data_inline[1].c_str());
			this_item_type.length = atoi(data_inline[0].c_str());
			Lists.item_types_list.push_back(this_item_type);
		}
	}

	// �����Ӱ尴�տ�ȴӿ�խ����
	ItemProperties  VP;
	int all_items_list_size = Lists.all_items_list.size();
	for (int i = 0; i < all_items_list_size - 1; i++)
	{
		for (int j = i + 1; j < all_items_list_size; j++)
		{
			if (Lists.all_items_list[i].width < Lists.all_items_list[j].width)
			{
				VP = Lists.all_items_list[i];
				Lists.all_items_list[i] = Lists.all_items_list[j];
				Lists.all_items_list[j] = VP;
			}
		}
	}
}