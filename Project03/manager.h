#ifndef MANAGER_H
#include "lib.h"
#include "node.h"

class Manager
{
	vector<string> inputNode;
	vector<string> outputNode;
	vector<FuncNode *> funcNode;

public:
	Manager();
	~Manager();
	bool ReadFile(const char* fileName);
	void MaxKernelSimplify();

private:
	bool processInput(ifstream &fileStream);
	void divideFunc(FuncNode* func, const set<set<string>>& divisor);
};

#endif