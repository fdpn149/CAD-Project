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

	vector<string> special_name;

	int input_literal_count = 0;
	int simplified_literal_count = 0;

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
	void addNewNode(SOP kernel);
};

#endif