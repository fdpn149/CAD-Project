#ifndef MANAGER_H
#include "lib.h"
#include "node.h"

class Manager
{
	vector<string> inputNode;
	vector<string> outputNode;
	vector<FuncNode *> funcNode;
	vector<KernelNode*> kernelNode;
	int newNodeCount = 0;
	
	string model_name;
	vector<string> input, output;

	int input_literal_count = 0;
	int simplified_literal_count = 0;

	set<Term> cokernel_exist;	//check exist
	vector<Term> cokernel;
	vector<SOP> kernel;
	vector<vector<int>> divideTerm;

	vector<Term> terms;
	unordered_map<string, vector<vector<string>>> names;
	unordered_map<string, set<string>> names_input;

public:
	Manager();
	~Manager();
	bool ReadFile(const char* fileName);
	void MaxKernelSimplify();
	void Simplify();
	void WriteFile();
private:
	bool processInput(ifstream &fileStream);
	void divideFunc(FuncNode* func, const SOP& divisor, Term& quotient);
	void addNewNode(SOP& kernel);
	bool reCalcCKernel(FuncNode* func, const SOP& kernel);	// return false if the kernel record been delete
	void detailSimplify();
	void findGlobalKernel(const int& col_current, const vector<int>& same_literal_row, vector<string>& matrix, vector<string>& literals, unordered_map<string, int>& literals_index);
};

#endif