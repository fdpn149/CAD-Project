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
	
	map<coKernel,Kernel> coKernel_kernel;

	static map<Kernel, KernelRecord> kernel_appear;
	
	void findAllKernel();
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