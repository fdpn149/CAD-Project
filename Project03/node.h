#ifndef NODE_H
#include "lib.h"
#include "kernelRecord.h"

typedef set<set<string>> SOP;
typedef set<string> Term;

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

	set<Term> cokernel_exist;	//check exist
	vector<Term> cokernel;
	vector<SOP> kernel;
	
	static map<SOP,int> kernelRecord_index;	//check exist
	static vector<KernelRecord> kernelRecord;

	void findAllKernel();
private:
	void findKernel(const int& col_current, const vector<int>& same_literal_row);
};

class KernelRecord
{
	struct KernelDetail
	{
		int count;
		Term coKernel;
		FuncNode* from;
	};
public:
	KernelRecord(FuncNode* f, const SOP& k, const Term& c);
	int cost = 0;
	SOP kernel;
	void add(FuncNode* f, Term& c);
	vector<KernelDetail> detail;
};

#endif