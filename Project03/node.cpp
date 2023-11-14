#include "node.h"

vector<KernelRecord> FuncNode::kernelRecord;

FuncNode::~FuncNode()
{
}

void FuncNode::findAllKernel()
{
	for (int col = 0; col < input.size(); col++)
	{
		vector<int> same_literal_row;

		vector<CoKernel> all_cokernel;

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
	CoKernel newCokernel;
	newCokernel.insert(input[col_current]);

	map<int, set<string>> newTempKernel;

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

	if (cokernel_exist.find(newCokernel) != cokernel_exist.end())
		return;

	Kernel newKernel;
	for (const auto& tempKernel : newTempKernel)
	{
		newKernel.insert(tempKernel.second);
	}

	cokernel_exist.insert(newCokernel);
	cokernel.push_back(newCokernel);
	kernel.push_back(newKernel);

	auto it = std::find_if(kernelRecord.begin(), kernelRecord.end(), [newKernel](KernelRecord kr)
		{return kr.kernel == newKernel; });

	if (it != kernelRecord.end())
	{
		it->add(this, newCokernel);
	}
	else
	{
		KernelRecord newKernelRecord(this, newKernel, newCokernel);
		kernelRecord.push_back(newKernelRecord);
	}
}

KernelRecord::KernelRecord(FuncNode* f, const Kernel& k, const CoKernel& c)
{
	kernel = k;
	cost = c.size() * (k.size() - 1) - 1;
	where_count.push_back(std::make_pair(f, cost));
}

void KernelRecord::add(FuncNode* f, CoKernel& c)
{
	cost += c.size() * (kernel.size() - 1) - 1;
	where_count.push_back(std::make_pair(f, cost));
}