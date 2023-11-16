#include "manager.h"

Manager::Manager()
{
}

Manager::~Manager()
{
	for (auto& node : funcNode)
		delete node;
	for (auto& node : kernelNode)
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

bool Manager::processInput(ifstream& fileStream)
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
					inputs.push_back(word);
				}
				fileStream >> word;
			}
			string output(*inputs.rbegin());
			inputs.pop_back();
			FuncNode* newFunc = new FuncNode(output);

			while (word != ".names" && word != ".end")
			{
				set<string> term;
				for (int i = 0; i < word.length(); i++)
				{
					if (word[i] == '1')
					{
						term.insert(inputs[i]);
						if (newFunc->input_index.find(inputs[i]) == newFunc->input_index.end())
						{
							newFunc->input_index[inputs[i]] = newFunc->input.size();
							newFunc->input.push_back(inputs[i]);
						}
					}
					else if (word[i] == '0')
					{
						string name = inputs[i] + "~";
						term.insert(name);
						if (newFunc->input_index.find(name) == newFunc->input_index.end())
						{
							newFunc->input_index[name] = newFunc->input.size();
							newFunc->input.push_back(name);
						}
					}
				}
				newFunc->function.push_back(term);
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
	auto max_record_it = FuncNode::kernelRecord.begin();
	auto temp_it = max_record_it;
	for (temp_it++; temp_it != FuncNode::kernelRecord.end(); temp_it++)
	{
		if (temp_it->second.cost > max_record_it->second.cost)
			max_record_it = temp_it;
	}

	addNewNode(max_record_it->second.kernel);

	bool status = true;

	map<FuncNode*, KernelRecord::KernelDetail>::iterator max_record_func;
	while (status == true && !max_record_it->second.detail.empty())
	{
		max_record_func = max_record_it->second.detail.begin();
		divideFunc(max_record_func->first, max_record_it->second.kernel, max_record_func->second.coKernel);
		status = reCalcCKernel(max_record_func->first, max_record_it->second.kernel);
	}
	newNodeCount++;

	/*for (auto max_it = FuncNode::kernelRecord.begin(); max_it != FuncNode::kernelRecord.end(); max_it++)
	{
		addNewNode(max_it->kernel);

		for (auto it = max_it->detail.begin(); it != max_it->detail.end(); it++)
		{
			divideFunc(it->from, max_it->kernel, it->coKernel);
		}
	}
	newNodeCount++;*/

}

void Manager::Simplify()
{
	while (!FuncNode::kernelRecord.empty())
	{
		MaxKernelSimplify();
	}
}

void Manager::divideFunc(FuncNode* func, const SOP& divisor, Term& quotient)
{
	vector<Term> dividend = func->function;

	if (divisor.size() > dividend.size())	// divisor has too many terms
		return;

	// form dividend name & matrix
	unordered_map<string, int> dividend_literal_index = func->input_index;
	vector<string> dividend_literal = func->input;

	string init(dividend_literal.size(), '0');

	int dividend_function_size = dividend.size();
	vector<string> dividend_matrix(dividend_function_size, init);

	for (int i = 0; i < dividend_function_size; i++)
	{
		for (const auto& literal : dividend[i])
		{
			dividend_matrix[i][dividend_literal_index.at(literal)] = '1';
		}
	}

	// form quotient
	string quotient_str(dividend_literal.size(), '0');

	for (const string& literal : quotient)
		quotient_str[dividend_literal_index.at(literal)] = '1';

	vector<Term> remainder;
	int divide_count = 0;
	bool finished = false;

	// divide
	for (int i = 0; i < dividend.size(); i++)	//term_set[i] => dividend_term
	{
		bool is_remainder = true;

		if (!finished)
		{
			for (const Term& divisor_term : divisor)
			{
				int divisor_term_size = divisor_term.size();
				bool found = false;
				if (divisor_term_size < dividend[i].size())
				{
					int count = 0;

					for (const string& dividend_literal : dividend[i])
					{
						for (const string& divisor_literal : divisor_term)
						{
							if (divisor_literal == dividend_literal)
							{
								count++;
								if (count == divisor_term_size)
								{
									found = true;
									break;
								}
							}
						}
						if (found)
							break;
					}
				}
				else if (divisor_term.size() == dividend[i].size() && divisor_term == dividend[i])
				{
					found = true;

				}
				else
					continue;

				if (found)
				{
					string dividend_copy(dividend_matrix[i]);

					for (const string& divisor_literal : divisor_term)
					{
						dividend_copy[dividend_literal_index.at(divisor_literal)] = '0';
					}

					if (dividend_copy == quotient_str)
					{
						is_remainder = false;
						divide_count++;

						if (divide_count == divisor.size())
							finished = true;

						break;
					}
				}
			}
		}

		if (is_remainder == true)
		{
			remainder.push_back(dividend[i]);
		}
	}

	quotient.insert("new" + std::to_string(newNodeCount));	// add the new kernel node
	vector<Term> newFunc(remainder.begin(), remainder.end());
	newFunc.push_back(quotient);
	func->function = newFunc;

	func->input_index.clear();
	func->input.clear();

	for (const auto& term : newFunc)
	{
		for (const auto& literal : term)
		{
			if (func->input_index.find(literal) == func->input_index.end())
			{
				func->input_index[literal] = func->input.size();
				func->input.push_back(literal);
			}
		}
	}
}

string Manager::addNewNode(SOP& kernel)
{
	KernelNode* newKernelNode = new KernelNode(newNodeCount, kernel);
	kernelNode.push_back(newKernelNode);
	return newKernelNode->getName();
}

bool Manager::reCalcCKernel(FuncNode* func, const SOP& kernel)
{
	bool status = true;

	for (int i = 0; i < func->kernel.size(); i++)
	{
		auto kernelRecord = FuncNode::kernelRecord[func->kernel[i]];
		if (kernelRecord.detail.size() > 1)
			kernelRecord.removeSource(func, func->cokernel[i].size());
		else
		{
			if(kernelRecord.kernel == kernel)
				status = false;
			FuncNode::kernelRecord.erase(func->kernel[i]);
		}
	}

	func->kernel.clear();
	func->cokernel.clear();
	func->cokernel_exist.clear();

	if (func->function.size() > 1)
		func->findAllKernel();

	return status;
}
