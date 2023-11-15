#include "node.h"

vector<KernelRecord> FuncNode::kernelRecord;
map<SOP, int> FuncNode::kernelRecord_index;

FuncNode::FuncNode(const string& name) : Node(name)
{
	
}

FuncNode::~FuncNode()
{
}

void FuncNode::findAllKernel()
{
	vector<string> matrix(function.size(), string(input.size(), '0'));

	for (int i = 0; i < function.size(); i++)
	{
		for (const string& literal : function[i])
		{
			matrix[i][input_index.at(literal)] = '1';
		}
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
	for (const auto& tempKernel : newTempKernel)
	{
		newKernel.insert(tempKernel.second);
	}

	cokernel_exist.insert(newCokernel);
	cokernel.push_back(newCokernel);
	kernel.push_back(newKernel);


	if (kernelRecord_index.find(newKernel) != kernelRecord_index.end())
	{
		kernelRecord.at(kernelRecord_index[newKernel]).add(this, newCokernel);
	}
	else
	{
		KernelRecord newKernelRecord(this, newKernel, newCokernel);
		kernelRecord_index[newKernel] = kernelRecord.size();
		kernelRecord.push_back(newKernelRecord);
	}
}

KernelRecord::KernelRecord(FuncNode* f, const SOP& k, const Term& c)
{
	kernel = k;
	cost = c.size() * (k.size() - 1) - 1;
	detail.push_back({cost, c, f});
}

void KernelRecord::add(FuncNode* f, Term& c)
{
	cost += c.size() * (kernel.size() - 1) - 1;
	detail.push_back({ cost, c, f });
}