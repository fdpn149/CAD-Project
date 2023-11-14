#ifndef MANAGER_H
#include "lib.h"
#include "node.h"

struct sortFunc
{
	bool operator() (pair<Kernel, KernelRecord> a, pair<Kernel, KernelRecord> b) const {
        return a.second.sum > b.second.sum;
    }
};

class Manager
{
	vector<string> inputNode;
	vector<string> outputNode;
	vector<FuncNode *> funcNode;
	vector<pair<Kernel, KernelRecord>> kernels;

public:
	Manager();
	~Manager();
	bool ReadFile(const char* fileName);
	
	void sortKernel();


private:
	bool processInput(ifstream &fileStream);
};

#endif