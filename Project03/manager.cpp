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

void Manager::sortKernel()
{
	//kernels.insert(kernels.begin(), FuncNode::kernel_appear.begin(), FuncNode::kernel_appear.end());
			//std::sort(kernels.begin(), kernels.end(), [](pair<Kernel, KernelRecord> a, pair<Kernel, KernelRecord> b){return a.second.sum > b.second.sum;});
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
				for (int i = 0; i < word.length(); i++)
				{
					if (word[i] == '1')
					{
						term.append("01");
					}
					else if (word[i] == '0')
					{
						term.append("10");
					}
					else if (word[i] == '-')
					{
						term.append("00");
					}
				}
				newFunc->term.push_back(term);
				fileStream >> word;
				fileStream >> word;
			}
			newFunc->findAllKernel();
			funcNode.push_back(newFunc);
		}
	}



	return true;
}
