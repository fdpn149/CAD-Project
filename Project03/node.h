#ifndef NODE_H
#include "lib.h"

typedef set<set<string>> Kernel;
typedef set<string> coKernel;
class KernelRecord;

class Node
{
public:
	string name;
};

class FuncNode : Node
{
public:
	FuncNode(const string &name) { this->name = name; }
	~FuncNode();
	vector<string> input;
	vector<string> term;
	
	vector<coKernel> cokernel;
	vector<Kernel> kernel;

	static map<Kernel, KernelRecord> kernel_appear;
	
	void findAllKernel();
private:
	void findKernel(const int& col_current, const vector<int>& same_literal_row);
};

class KernelRecord
{
	set<pair<FuncNode*, int>> from_count;
public:
	KernelRecord(){}
	int sum = 0;
	void insert(FuncNode* func, const Kernel& kernel, const coKernel& coKernel);
};


#endif