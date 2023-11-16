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
public:
	Manager();
	~Manager();
	bool ReadFile(const char* fileName);
	void MaxKernelSimplify();
	void Simplify();
private:
	bool processInput(ifstream &fileStream);
	void divideFunc(FuncNode* func, const SOP& divisor, Term& quotient);
	string addNewNode(SOP& kernel);
	bool reCalcCKernel(FuncNode* func, const SOP& kernel);	// return false if the kernel record been delete
};

#endif