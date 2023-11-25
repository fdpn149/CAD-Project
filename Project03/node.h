#ifndef NODE_H
#include "lib.h"

typedef set<set<string>> SOP;
typedef set<string> Term;

class KernelRecord;

class Node
{
public:
	Node(string n) : name(n) {}
	string name;
};

class KernelNode : Node
{
public:
	KernelNode(int count, SOP& func) : Node("new" + std::to_string(count)), function(func) { }
	SOP function;
	string getName() const { return name; };
};

class FuncNode : Node
{
public:
	FuncNode(const string& name);
	~FuncNode();
	vector<string> input;
	unordered_map<string, int> input_index;	//name=>index

	set<Term> function;

	set<Term> cokernel_exist;	//check exist
	vector<Term> cokernel;
	vector<SOP> kernel;

	static map<SOP, KernelRecord> kernelRecord;

	void findAllKernel();
	string getName() const { return name; };
private:
	void findKernel(const int& col_current, const vector<int>& same_literal_row, vector<string>& matrix);
	void addKernelRecord(const SOP& kernel, const Term& coKernel);
};

class KernelRecord
{
public:
	struct KernelDetail
	{
		int count = 0;
		Term coKernel;
	};
	KernelRecord(FuncNode* f, const SOP& k, const Term& c);
	KernelRecord(){}
	int cost = 0;
	SOP kernel;
	void add(FuncNode* f, const Term& c);
	map<FuncNode*, KernelDetail> detail;
	void removeSource(FuncNode* func, const int& cok_size);
};

#endif