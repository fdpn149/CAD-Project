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

		vector<coKernel> all_cokernel;

		for (int row = 0; row < term.size(); row++)
		{
			if (term[row][col] == '1')
				same_literal_row.push_back(row);
		}
		if (same_literal_row.size() > 1)
		{
			findKernel(col, same_literal_row);
		}
	}
}

void FuncNode::findKernel(const int& col_current, const vector<int>& same_literal_row)
{
	coKernel newCokernel;
	newCokernel.insert(input[col_current]);

	map<int,set<string>> newTempKernel;

	for (int column = 0; column < input.size(); column++)
	{
		if (column == col_current)
			continue;

		bool allone = true;
		vector<int> one;

		int i;

		for (i = 0; i < same_literal_row.size(); i++)	//not the same as the 'row' outside
		{
			if (term[same_literal_row[i]][column] == '1')
			{
				one.push_back(same_literal_row[i]);
			}
			else	//0
			{
				allone = false;
			}
		}
		if (allone)
		{
			newCokernel.insert(input[column]);
		}
		else
		{
			for (const int& i_one : one)
			{
				newTempKernel[i_one].insert(input[column]);
			}
		}
	}

	

	Kernel newKernel;
	for (const auto& tempKernel : newTempKernel)
	{
		newKernel.insert(tempKernel.second);
	}

	cokernel.push_back(newCokernel);
	kernel.push_back(newKernel);
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
