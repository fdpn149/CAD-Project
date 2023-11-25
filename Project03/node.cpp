#include "node.h"

map<SOP, KernelRecord> FuncNode::kernelRecord;

FuncNode::FuncNode(const string& name) : Node(name)
{
	
}

FuncNode::~FuncNode()
{
}

void FuncNode::findAllKernel()
{
	vector<string> matrix(function.size(), string(input.size(), '0'));

	int index = 0;
	for(const Term& term : function)
	{
		for (const string& literal : term)
		{
			matrix[index][input_index.at(literal)] = '1';
		}
		index++;
	}


	for (int col = 0; col < input.size(); col++)
	{
		vector<int> same_literal_row;

		vector<Term> all_cokernel;

		for (int row = 0; row < function.size(); row++)
		{
			if (matrix[row][col] == '1')
				same_literal_row.push_back(row);
		}
		if (same_literal_row.size() > 1)
		{
			findKernel(col, same_literal_row, matrix);
		}
	}
}

void FuncNode::findKernel(const int& col_current, const vector<int>& same_literal_row, vector<string>& matrix)
{
	Term newCokernel;
	newCokernel.insert(input[col_current]);

	map<int, set<string>> newTempKernel;

	for (int column = 0; column < input.size(); column++)
	{
		if (column == col_current)
			continue;

		bool allone = true;
		vector<int> one;	// index of row which the literal equal to 1

		for (int row_in_slr : same_literal_row)
		{
			if (matrix[row_in_slr][column] == '1')
			{
				one.push_back(row_in_slr);
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

	SOP newKernel;
	for (const pair<const int, Term>& tempKernel : newTempKernel)
	{
		newKernel.insert(tempKernel.second);
	}

	cokernel_exist.insert(newCokernel);
	cokernel.push_back(newCokernel);
	kernel.push_back(newKernel);

	addKernelRecord(newKernel, newCokernel);
}

void FuncNode::addKernelRecord(const SOP& kernel, const Term& coKernel)
{
	map<SOP,KernelRecord>::iterator it = kernelRecord.find(kernel);
	if (it != kernelRecord.end())
	{
		if (it->second.detail.find(this) == it->second.detail.end())
			it->second.add(this, coKernel);
	}
	else
	{
		KernelRecord newKernelRecord(this, kernel, coKernel);
		kernelRecord[kernel] = newKernelRecord;
	}
}

KernelRecord::KernelRecord(FuncNode* f, const SOP& k, const Term& c)
{
	kernel = k;
	cost = (int)c.size() * ((int)k.size() - 1) - 1;
	detail[f] = {cost, c};
}

void KernelRecord::add(FuncNode* f, const Term& c)
{
	int new_cost = (int)c.size() * ((int)kernel.size() - 1) - 1;
	detail[f] = { new_cost, c };
	cost += (int)c.size() * ((int)kernel.size() - 1) - 1;
}

void KernelRecord::removeSource(FuncNode* func, const int& cok_size)
{
	cost -= cok_size * ((int)kernel.size() - 1) - 1;
	detail.erase(func);
}
