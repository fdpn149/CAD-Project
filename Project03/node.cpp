#include "node.h"

map<Kernel, KernelRecord> FuncNode::kernel_appear;

FuncNode::~FuncNode()
{
}

void FuncNode::findAllKernel()
{
	for (int col = 0; col < input.size(); col++)
	{
		vector<int> same_literal_row;
		coKernel coKernel_current;

		for (int row = 0; row < term.size(); row++)
		{
			if (term[row][col] == '1')
				same_literal_row.push_back(row);
		}
		if (same_literal_row.size() > 1)
		{
			coKernel_current.insert(input[col]);
			vector<string> common_term;
			for (int n : same_literal_row) // distinct original term because I will modifiy it
			{
				string will_push = term[n];
				will_push[col] = '0';
				common_term.push_back(will_push);
			}

			vector<set<string>> kernel_current;
			kernel_current.resize(common_term.size());
			for (int col_com = 0; col_com < input.size(); col_com++) // common_term's column
			{
				int row;
				for (row = 0; row < common_term.size(); row++)
				{
					if (common_term[row][col_com] == '0')
						break;
				}
				if (row >= common_term.size())
					coKernel_current.insert(input[col_com]);
				else
				{
					for (row = 0; row < common_term.size(); row++)
					{
						if (common_term[row][col_com] == '1')
							kernel_current[row].insert(input[col_com]);
					}
				}
			}
			Kernel kernel_current_sort(kernel_current.begin(), kernel_current.end());
			coKernel_kernel[coKernel_current] = kernel_current_sort;
			if (kernel_appear.find(kernel_current_sort) == kernel_appear.end())
			{
				KernelRecord newRecord;
				newRecord.insert(this, kernel_current_sort, coKernel_current);
				kernel_appear[kernel_current_sort] = newRecord;
			}
			else
				kernel_appear[kernel_current_sort].insert(this, kernel_current_sort, coKernel_current);
		}
	}
}

void KernelRecord::insert(FuncNode *func, const Kernel &kernel, const coKernel &coKernel)
{
	int cost = (kernel.size() - 1) * coKernel.size() - 1;
	auto from_pair = std::make_pair(func, cost);
	if (from_count.find(from_pair) == from_count.end())
	{
		from_count.insert(from_pair);
		sum += cost;
	}
}
