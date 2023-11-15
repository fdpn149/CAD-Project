#include "manager.h"

Manager::Manager()
{
}

Manager::~Manager()
{
	for (auto& node : funcNode)
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
					inputs.push_back(word + "~");
					inputs.push_back(word);
				}
				fileStream >> word;
			}
			string output(*inputs.rbegin());
			inputs.pop_back();
			inputs.pop_back();
			FuncNode* newFunc = new FuncNode(output);
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
	vector<KernelRecord>::iterator max_it = FuncNode::kernelRecord.begin() + 0;
	/*for (auto it = max_it + 1; it != FuncNode::kernelRecord.end(); it++)
	{
		if (it->cost > max_it->cost)
			max_it = it;
	}*/

	for (; max_it != FuncNode::kernelRecord.end(); max_it++)
	{
		for (auto it = max_it->detail.begin(); it != max_it->detail.end(); it++)
		{
			divideFunc(it->from, max_it->kernel, it->coKernel);
		}
	}
}

void Manager::divideFunc(FuncNode* func, const SOP& divisor, Term& quotient)
{
	vector<set<string>>& term_set = func->term_set;

	if (divisor.size() > term_set.size())	// divisor has too many terms
		return;

	// form dividend name & matrix
	int index_count = 0;
	unordered_map<string, int> dividend_name_index;
	vector<string> dividend_name;

	for (const auto& i_term : term_set)
	{
		for (const auto& literal : i_term)
		{
			if (dividend_name_index.find(literal) == dividend_name_index.end())
			{
				dividend_name_index[literal] = index_count;
				dividend_name.push_back(literal);
				index_count++;
			}
		}
	}

	string init(index_count, '0');

	int term_set_size = term_set.size();
	vector<string> dividend_matrix(term_set_size, init);

	for (int i = 0; i < term_set_size; i++)
	{
		for (const auto& literal : term_set[i])
		{
			dividend_matrix[i][dividend_name_index.at(literal)] = '1';
		}
	}

	// form quotient
	string quotient_str(index_count, '0');

	for (const string& literal : quotient)
		quotient_str[dividend_name_index.at(literal)] = '1';

#pragma region FORM_DIVISOR
	// form divisor name & matrix
	/*index_count = 0;
	unordered_map<string, int> divisor_name_index;
	for (const auto& i_term : divisor)
	{
		for (const auto& literal : i_term)
		{
			if (divisor_name_index.find(literal) == divisor_name_index.end())
			{
				divisor_name_index[literal] = index_count;
				index_count++;
			}
		}
	}

	string init2(index_count, '0');

	int divisor_size = divisor.size();
	vector<string> divisor_matrix(divisor_size, init2);

	int matrix_index = 0;
	for (auto it = divisor.begin(); it != divisor.end(); it++)
	{
		for (const auto& literal : *it)
		{
			divisor_matrix[matrix_index][divisor_name_index.at(literal)] = '1';
		}
		matrix_index++;
	}*/
#pragma endregion

	vector<Term> remainder;
	int divide_count = 0;
	bool finished = false;

	// divide
	for (int i = 0; i < term_set.size(); i++)	//term_set[i] => dividend_term
	{
		bool is_remainder = true;

		if (!finished)
		{
			for (const Term& divisor_term : divisor)
			{
				int divisor_term_size = divisor_term.size();
				bool found = false;
				if (divisor_term_size < term_set[i].size())
				{
					int count = 0;

					for (const string& dividend_literal : term_set[i])
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
				else if (divisor_term.size() == term_set[i].size() && divisor_term == term_set[i])
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
						dividend_copy[dividend_name_index.at(divisor_literal)] = '0';
					}

					if (dividend_copy == quotient_str)
					{
						printf("found\n");
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
			printf("remain\n");
			remainder.push_back(term_set[i]);
		}
	}
}
