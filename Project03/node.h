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

class FuncNode : Node
{
public:
	FuncNode(const string &name) { this->name = name; }
	~FuncNode();
	vector<string> input;
	vector<string> term;
	
	vector<set<string>> term_set;

	set<CoKernel> cokernel_exist;	//check exist
	vector<CoKernel> cokernel;
	vector<Kernel> kernel;
	
	static map<Kernel,int> kernelRecord_index;	//check exist
	static vector<KernelRecord> kernelRecord;

	void findAllKernel();
private:
	void findKernel(const int& col_current, const vector<int>& same_literal_row);
};

class KernelRecord
{
public:
	KernelRecord(FuncNode* f, const Kernel& k, const CoKernel& c);
	int cost = 0;
	Kernel kernel;
	void add(FuncNode* f, CoKernel& c);
	vector<pair<FuncNode*, int>> where_count;
};

#endif