#ifndef NODE_H
#include "lib.h"
#include "kernelRecord.h"

typedef set<set<string>> Kernel;
typedef set<string> CoKernel;

class KernelRecord;

class Node
{
public:
	string name;
};

struct cmp {
	bool operator() (KernelRecord a, KernelRecord b) const {
		return a.cost > b.cost;
	}
};

class FuncNode : Node
{
public:
	FuncNode(const string &name) { this->name = name; }
	~FuncNode();
	vector<string> input;
	vector<string> term;
	
	set<CoKernel> cokernel_exist;	//check repeat
	vector<CoKernel> cokernel;
	vector<Kernel> kernel;
	
	static vector<KernelRecord> kernelRecord;

	void findAllKernel();
private:
	void findKernel(const int& col_current, const vector<int>& same_literal_row);
};

class KernelRecord
{
	vector<pair<FuncNode*, int>> where_count;
public:
	KernelRecord(FuncNode* f, const Kernel& k, const CoKernel& c);
	int cost = 0;
	Kernel kernel;
	void add(FuncNode* f, CoKernel& c);
};

#endif