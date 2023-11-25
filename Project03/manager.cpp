#include "manager.h"

Manager::Manager()
{
}

Manager::~Manager()
{
	for (FuncNode*& node : funcNode)
		delete node;
	for (KernelNode*& node : kernelNode)
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
	{
		fileStream >> word;
		model_name = word;
	}
	fileStream >> word;
	if (word == ".inputs")
	{
		fileStream >> word;
		while (word != ".outputs")
		{
			if (word != "\\")
			{
				inputNode.push_back(word);
				input.push_back(word);
			}
			fileStream >> word;
		}
	}
	if (word == ".outputs")
	{
		fileStream >> word;
		while (word != ".names")
		{
			if (word != "\\")
			{
				outputNode.push_back(word);
				output.push_back(word);
			}
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
					if (word == ".names")
					{
						break;
					}
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
						input_literal_count++;
						term.insert(inputs[i]);
						if (newFunc->input_index.find(inputs[i]) == newFunc->input_index.end())
						{
							newFunc->input_index[inputs[i]] = (int)newFunc->input.size();
							newFunc->input.push_back(inputs[i]);
						}
					}
					else if (word[i] == '0')
					{
						input_literal_count++;
						string name = inputs[i] + "~";
						term.insert(name);
						if (newFunc->input_index.find(name) == newFunc->input_index.end())
						{
							newFunc->input_index[name] = (int)newFunc->input.size();
							newFunc->input.push_back(name);
						}
					}
				}
				newFunc->function.insert(term);
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
	if (FuncNode::kernelRecord.empty())
		return;

	vector<SOP> to_delete_record;	// will delete in the end


	map<SOP, KernelRecord>::iterator record_it = FuncNode::kernelRecord.begin();
	map<SOP, KernelRecord>::iterator max_record_it = FuncNode::kernelRecord.begin();

	for (record_it++; record_it != FuncNode::kernelRecord.end(); record_it++)
	{
		if (record_it->second.cost > max_record_it->second.cost)
			max_record_it = record_it;
	}

	addNewNode(max_record_it->first);

	for (map<FuncNode*, KernelRecord::KernelDetail>::iterator func_detail_it = max_record_it->second.detail.begin(); func_detail_it != max_record_it->second.detail.end(); func_detail_it++)
	{
		divideFunc(func_detail_it->first, max_record_it->first, func_detail_it->second.coKernel);

		for (SOP& kernel_in_func : func_detail_it->first->kernel)
		{
			if (!std::equal(kernel_in_func.begin(), kernel_in_func.end(), max_record_it->first.begin()))
			{
				if (FuncNode::kernelRecord[kernel_in_func].detail.size() > 1)
					FuncNode::kernelRecord[kernel_in_func].detail.erase(func_detail_it->first);
				else
					to_delete_record.push_back(kernel_in_func);
			}
		}

		func_detail_it->first->cokernel.clear();
		func_detail_it->first->cokernel_exist.clear();
		func_detail_it->first->kernel.clear();

		if (func_detail_it->first->function.size() > 1)
			func_detail_it->first->findAllKernel();
	}
	newNodeCount++;

	FuncNode::kernelRecord.erase(max_record_it);

	for (const SOP& record : to_delete_record)
	{
		FuncNode::kernelRecord.erase(record);
	}
}

void Manager::Simplify()
{
	while (!FuncNode::kernelRecord.empty())
	{
		MaxKernelSimplify();
	}
	detailSimplify();
}

void Manager::WriteFile()
{
	ofstream outputStream("out.blif");
	outputStream << ".model " << model_name << endl << ".inputs";
	for (const string& s : input)
		outputStream << " " << s;
	outputStream << endl << ".outputs";
	for (const string& s : output)
		outputStream << " " << s;

	for (const pair<const string,vector<vector<string>>>& funcs : names)
	{
		outputStream << endl << ".names";

		unordered_map<string, int> final_inputs_index;
		vector<string> final_inputs;

		for (const string& func_input : names_input[funcs.first])
		{
			if (*func_input.rbegin() == '~')
			{
				string name = func_input.substr(0, func_input.size() - 1);
				if (final_inputs_index.find(name) == final_inputs_index.end())
				{
					final_inputs_index[name] = (int)final_inputs.size();
					final_inputs.push_back(name);
				}
			}
			else if (final_inputs_index.find(func_input) == final_inputs_index.end())
			{
				final_inputs_index[func_input] = (int)final_inputs.size();
				final_inputs.push_back(func_input);
			}
		}

		for (const string& final_input : final_inputs)
		{
			outputStream << " " << final_input;
		}

		outputStream << " " << funcs.first;

		for (const vector<string>& term : funcs.second)
		{
			string s(final_inputs.size(), '-');
			for (const string& literal : term)
			{
				if (*literal.rbegin() == '~')
				{
					simplified_literal_count++;
					s[final_inputs_index.at(literal.substr(0, (int)literal.size() - 1))] = '0';
				}
				else
				{
					simplified_literal_count++;
					s[final_inputs_index.at(literal)] = '1';
				}
			}
			outputStream << endl << s << " 1";
		}
	}

	for (const KernelNode* funcs : kernelNode)
	{
		outputStream << endl << ".names";

		unordered_map<string, int> final_inputs_index;
		vector<string> final_inputs;

		for (const Term& term : funcs->function)
		{
			for (const string& literal : term)
			{
				if (*literal.rbegin() == '~')
				{
					string name = literal.substr(0, literal.size() - 1);
					if (final_inputs_index.find(name) == final_inputs_index.end())
					{
						final_inputs_index[name] = (int)final_inputs.size();
						final_inputs.push_back(name);
					}
				}
				else if (final_inputs_index.find(literal) == final_inputs_index.end())
				{
					final_inputs_index[literal] = (int)final_inputs.size();
					final_inputs.push_back(literal);
				}
			}
		}

		for (const string& final_input : final_inputs)
		{
			outputStream << " " << final_input;
		}

		outputStream << " " << funcs->getName();

		for (const Term& term : funcs->function)
		{
			string s(final_inputs.size(), '-');
			for (const string& literal : term)
			{
				if (*literal.rbegin() == '~')
				{
					simplified_literal_count++;
					s[final_inputs_index.at(literal.substr(0, (int)literal.size() - 1))] = '0';
				}
				else
				{
					simplified_literal_count++;
					s[final_inputs_index.at(literal)] = '1';
				}
			}
			outputStream << endl << s << " 1";
		}
	}

	outputStream << endl << ".end" << endl;

	printf("Original literal count: %d\n", input_literal_count);
	printf("Optimized literal count: %d\n", simplified_literal_count);
}

void Manager::divideFunc(FuncNode* func, const SOP& divisor, Term& quotient)
{
	set<Term> dividend = func->function;

	if (divisor.size() > dividend.size())	// divisor has too many terms
		return;

	// form dividend name & matrix
	unordered_map<string, int> dividend_literal_index = func->input_index;
	vector<string> dividend_literal = func->input;

	string init(dividend_literal.size(), '0');

	int dividend_function_size = (int)dividend.size();
	vector<string> dividend_matrix(dividend_function_size, init);

	int index = 0;
	for (const Term& dividend_term : dividend)
	{
		for (const string& literal : dividend_term)
		{
			dividend_matrix[index][dividend_literal_index.at(literal)] = '1';
		}
		index++;
	}

	// form quotient
	string quotient_str(dividend_literal.size(), '0');

	for (const string& literal : quotient)
		quotient_str[dividend_literal_index.at(literal)] = '1';

	vector<Term> remainder;
	int divide_count = 0;
	bool finished = false;

	index = 0;
	// divide
	for (const Term& dividend_term : dividend)
	{
		bool is_remainder = true;

		if (!finished)
		{
			for (const Term& divisor_term : divisor)
			{
				int divisor_term_size = (int)divisor_term.size();
				bool found = false;
				if (divisor_term_size < dividend_term.size())
				{
					int count = 0;

					for (const string& dividend_literal : dividend_term)
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
				else if (divisor_term.size() == dividend_term.size() && divisor_term == dividend_term)
				{
					found = true;

				}
				else
					continue;

				if (found)
				{
					string dividend_copy(dividend_matrix[index]);

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
			remainder.push_back(dividend_term);
		}
		index++;
	}

	quotient.insert("new" + std::to_string(newNodeCount));	// add the new kernel node
	set<Term> newFunc(remainder.begin(), remainder.end());
	newFunc.insert(quotient);
	func->function = newFunc;

	func->input_index.clear();
	func->input.clear();

	for (const Term& term : newFunc)
	{
		for (const string& literal : term)
		{
			if (func->input_index.find(literal) == func->input_index.end())
			{
				func->input_index[literal] = (int)func->input.size();
				func->input.push_back(literal);
			}
		}
	}
}

void Manager::addNewNode(SOP kernel)
{
	KernelNode* newKernelNode = new KernelNode(newNodeCount, kernel);
	kernelNode.push_back(newKernelNode);
}

void Manager::findGlobalKernel(const int& col_current, const vector<int>& same_literal_row, vector<string>& matrix, vector<string>& literals, unordered_map<string, int>& literals_index)
{
	Term newCokernel;
	newCokernel.insert(literals[col_current]);

	map<int, set<string>> newTempKernel;

	for (int column = 0; column < literals.size(); column++)
	{
		if (column == col_current || literals[column].substr(0, 5) == "extra")
			continue;

		bool allone = true;
		vector<int> one;	// index of row which the literal equal to 1

		for (int row_in_slr : same_literal_row)
		{
			if (matrix[row_in_slr][column] == '1')
			{
				one.push_back(row_in_slr);
			}
			else	//0
			{
				allone = false;
			}
		}
		if (allone)
		{
			newCokernel.insert(literals[column]);
		}
		else
		{
			for (const int& i_one : one)
			{
				newTempKernel[i_one].insert(literals[column]);
			}
		}
	}

	if (cokernel_exist.find(newCokernel) != cokernel_exist.end())
		return;

	SOP newKernel;
	for (const pair<const int, Term>& tempKernel : newTempKernel)
	{
		newKernel.insert(tempKernel.second);
	}

	cokernel_exist.insert(newCokernel);
	cokernel.push_back(newCokernel);
	kernel.push_back(newKernel);
	divideTerm.push_back(same_literal_row);
}

void Manager::detailSimplify()
{
	vector<string> literals;
	unordered_map<string, int> literals_index;


	for (int i = 0; i < funcNode.size(); i++)
	{
		string extra_col = "extra" + funcNode[i]->getName();
		literals_index[extra_col] = (int)literals.size();
		literals.push_back(extra_col);

		if (funcNode[i]->function.size() == 0)
		{
			Term dummyTerm;
			dummyTerm.insert(extra_col);
			terms.push_back(dummyTerm);
		}
		else
		{
			for (const Term& term : funcNode[i]->function)
			{
				for (const string& literal : term)
				{
					if (literals_index.find(literal) == literals_index.end())
					{
						literals_index[literal] = (int)literals.size();
						literals.push_back(literal);
					}
				}
				Term term_to_push(term);
				term_to_push.insert(extra_col);
				terms.push_back(term_to_push);
			}
		}
	}

	bool modify;

	do {
		modify = false;
		cokernel.clear();
		kernel.clear();
		cokernel_exist.clear();
		divideTerm.clear();
		vector<string> matrix(terms.size(), string(literals.size(), '0'));

		for (int i = 0; i < terms.size(); i++)
		{
			for (const string& literal : terms[i])
			{
				matrix[i][literals_index.at(literal)] = '1';
			}
		}


		for (int col = 0; col < literals.size(); col++)
		{
			if (literals[col].substr(0, 5) == "extra")
				continue;

			vector<int> same_literal_row;

			vector<Term> all_cokernel;

			for (int row = 0; row < terms.size(); row++)
			{
				if (matrix[row][col] == '1')
					same_literal_row.push_back(row);
			}
			if (same_literal_row.size() > 1)
			{
				findGlobalKernel(col, same_literal_row, matrix, literals, literals_index);
			}
		}

		if (!cokernel.empty())
		{
			int max = (int)kernel[0].size() * ((int)cokernel[0].size() - 1) - (int)cokernel[0].size();
			int max_index = 0;

			for (int i = 1; i < cokernel.size(); i++)
			{
				int num = (int)kernel[i].size() * ((int)cokernel[i].size() - 1) - (int)cokernel[i].size();
				if (num > max)
				{
					max = num;
					max_index = i;
				}
			}

			if (max > 0)
			{
				modify = true;
				SOP sop({ cokernel[max_index] });
				addNewNode(sop);
				string newNode_name = "new" + std::to_string(newNodeCount);
				literals_index[newNode_name] = (int)literals.size();
				literals.push_back(newNode_name);


				for (int index : divideTerm[max_index])
				{
					set<string> difference;
					std::set_difference(terms[index].begin(), terms[index].end(), cokernel[max_index].begin(), cokernel[max_index].end(), std::inserter(difference, difference.end()));
					if ((int)difference.size() != (int)terms[index].size() - (int)cokernel[max_index].size())
						printf("Fatal ERROR\n");
					difference.insert("new" + std::to_string(newNodeCount));
					terms[index] = difference;
				}
				newNodeCount++;
			}
		}

	} while (modify);
	for (const Term& term : terms)
	{
		vector<string> in;
		string out;
		set<string> in_set;
		for (const string& literal : term)
		{
			if (literal.substr(0, 5) == "extra")
			{
				out = literal.substr(5);
			}
			else
			{
				in.push_back(literal);
				in_set.insert(literal);
			}
		}
		if (!in.empty())
		{
			names[out].push_back(in);
			names_input[out].insert(in_set.begin(), in_set.end());
		}
		else
			names[out];
	}
}