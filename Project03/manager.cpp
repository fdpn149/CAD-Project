#include "manager.h"

Manager::Manager()
{
}

Manager::~Manager()
{
	for(auto& node : funcNode)
		delete node;
}

bool Manager::ReadFile(const char* fileName)
{
	ifstream fileStream;
	fileStream.open(fileName);
	if (!fileStream.is_open() || !processInput(fileStream))
		return false;

	fileStream.close();
	return true;
}

bool Manager::processInput(ifstream &fileStream)
{
	string word;
	fileStream >> word;
	if (word == ".model")
		fileStream >> word;
	fileStream >> word;
	if (word == ".inputs")
	{
		fileStream >> word;
		while (word != ".outputs")
		{
			if (word != "\\")
				inputNode.push_back(word);
			fileStream >> word;
		}
	}
	if (word == ".outputs")
	{
		fileStream >> word;
		while (word != ".names")
		{
			if (word != "\\")
				outputNode.push_back(word);
			fileStream >> word;
		}
	}
	if (word == ".names")
	{
		while (word != ".end")
		{
			fileStream >> word;
			vector<string> inputs;
			while (word[0] != '0' && word[0] != '1' && word[0] != '-')
			{
				if (word != "\\")
				{
					inputs.push_back(word + "'");
					inputs.push_back(word);
				}
				fileStream >> word;
			}
			string output(*inputs.rbegin());
			inputs.pop_back();
			inputs.pop_back();
			FuncNode *newFunc = new FuncNode(output);
			newFunc->input = inputs;

			while (word != ".names" && word != ".end")
			{
				string term;
				set<string> term_set;
				for (int i = 0; i < word.length(); i++)
				{
					if (word[i] == '1')
					{
						term.append("01");
						term_set.insert(inputs[2 * i + 1]);
					}
					else if (word[i] == '0')
					{
						term.append("10");
						term_set.insert(inputs[2 * i]);
					}
					else if (word[i] == '-')
					{
						term.append("00");
					}
				}
				newFunc->term.push_back(term);
				newFunc->term_set.push_back(term_set);
				fileStream >> word;
				fileStream >> word;
			}
			newFunc->findAllKernel();
			funcNode.push_back(newFunc);
		}
	}



	return true;
}

void Manager::MaxKernelSimplify()
{
	// find max cost
	vector<KernelRecord>::iterator max_it = FuncNode::kernelRecord.begin();
	for (auto it = max_it + 1; it != FuncNode::kernelRecord.end(); it++)
	{
		if (it->cost > max_it->cost)
			max_it = it;
	}

	for (vector<pair<FuncNode*, int>>::iterator it = max_it->where_count.begin(); it != max_it->where_count.end(); it++)
	{
		divideFunc(it->first, max_it->kernel);
	}
}

void Manager::divideFunc(FuncNode* func, const set<set<string>>& divisor)
{

}
