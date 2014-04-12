#include <fstream>
#include <vector>
#include <string>
using namespace std;

class Automata
{
	vector<int>acceptStates;
	int initialState;
	int nrOfStates;
	int nrOfAcceptStates;
	int nrOfTransitions;
	string** transitionTable;
	bool isInGNFAFInalState;
public:
	Automata::Automata(ifstream&);
	Automata(const Automata&);
	void convertToRegex();
	void convertToGNFAInitial();
	void convertToGNFAFinal();
	void shift(int);
	void addEmptyState();
	void addTransition(string, int, int);
	void outputRegex(ofstream&);
};

Automata::Automata(ifstream& fin)
{
	isInGNFAFInalState = false;
	fin >> nrOfStates;
	fin >> nrOfTransitions;

	transitionTable = new string*[nrOfStates];
	for (int i = 0; i < nrOfStates; ++i)
		transitionTable[i] = new string[nrOfStates]();
	
	for (int i = 0; i < nrOfTransitions; ++i)
	{
		int state1, state2;
		string value;
		fin >> state1 >> value >> state2;
		transitionTable[state1][state2] = value;
	}

	fin >> initialState;
	fin >> nrOfAcceptStates;
	for (int i = 0; i < nrOfAcceptStates; ++i)
	{
		int state;
		fin >> state;
		acceptStates.push_back(state);
	}
}

Automata::Automata(const Automata& objectArg)
{
	nrOfStates = objectArg.nrOfStates;
	initialState = objectArg.initialState;
	nrOfTransitions = objectArg.nrOfTransitions;
	nrOfAcceptStates = objectArg.nrOfAcceptStates;
	acceptStates = objectArg.acceptStates;

	transitionTable = new string*[nrOfStates];
	for (int i = 0; i < nrOfStates; ++i)
		transitionTable[i] = new string[nrOfStates]();

	for (int i = 0; i < nrOfStates; ++i)
	for (int j = 0; j < nrOfStates; ++j)
		transitionTable[i][j] = objectArg.transitionTable[i][j];
}

void Automata::addTransition(string value, int state1Arg, int state2Arg)
{
	if (state1Arg < nrOfStates && state2Arg < nrOfStates)
		transitionTable[state1Arg][state2Arg] = value;
}

void Automata::shift(int shiftArg)
{
	int newNrOfStates = nrOfStates + shiftArg;

	string** newTransitionTable = new string*[newNrOfStates];
	for (int i = 0; i < newNrOfStates; ++i)
		newTransitionTable[i] = new string[newNrOfStates]();

	for (int i = 0; i < nrOfStates; ++i)
	for (int j = 0; j < nrOfStates; ++j)
		newTransitionTable[i + shiftArg][j + shiftArg] = transitionTable[i][j];

	for (int i = 0; i < nrOfStates; ++i)
		delete[] transitionTable[i];

	transitionTable = newTransitionTable;
	nrOfStates = newNrOfStates;
	initialState = initialState + shiftArg;
	
	for (int i = 0; i < nrOfAcceptStates; ++i)
		acceptStates[i] += shiftArg;
}

void Automata::addEmptyState()
{
	string** newTransitionTable = new string*[nrOfStates + 1];
	for (int i = 0; i < nrOfStates + 1; ++i)
		newTransitionTable[i] = new string[nrOfStates + 1]();

	for (int i = 0; i < nrOfStates; ++i)
	for (int j = 0; j < nrOfStates; ++j)
		newTransitionTable[i][j] = transitionTable[i][j];

	for (int i = 0; i < nrOfStates; ++i)
		delete[] transitionTable[i];

	transitionTable = newTransitionTable;
	nrOfStates = nrOfStates + 1;
}

void Automata::convertToRegex()
{
	convertToGNFAInitial();
	convertToGNFAFinal();
}

void Automata::convertToGNFAInitial()
{
	shift(1);
	addTransition("E", 0, initialState);
	initialState = 0;

	addEmptyState();
	for (int i = 0; i < nrOfAcceptStates; ++i)
		addTransition("E", acceptStates[i], nrOfStates - 1);
	
	acceptStates.clear();
	acceptStates.push_back(nrOfStates - 1);
}

void Automata::convertToGNFAFinal()
{
	for (int k = 1; k < nrOfStates - 1; ++k)
	{
		for (int i = 0; i < nrOfStates; ++i)
		for (int j = 0; j < nrOfStates; ++j)
		if (transitionTable[i][k] != "" && transitionTable[k][j] != "" && i != k && j != k)
		{
			if (transitionTable[k][k] != "" && transitionTable[i][j] != "")
				transitionTable[i][j] = transitionTable[i][j] + "|" + "(" +transitionTable[i][k] + "(" + transitionTable[k][k] + ")" + "*" + transitionTable[k][j] + ")";
			else if (transitionTable[k][k] != "" && transitionTable[i][j] == "")
				transitionTable[i][j] = transitionTable[i][k] + "(" + transitionTable[k][k] + ")" + "*" + transitionTable[k][j];
			else if (transitionTable[k][k] == "" && transitionTable[i][j] != "")
				transitionTable[i][j] = transitionTable[i][j] + "|" + "(" + transitionTable[i][k] + transitionTable[k][j] + ")";
			else if (transitionTable[k][k] == "" && transitionTable[i][j] == "")
				transitionTable[i][j] = transitionTable[i][k] + transitionTable[k][j];

			for (int q = 0; q < transitionTable[i][j].length(); ++q)
			if (transitionTable[i][j][q] == 'E')
				transitionTable[i][j].erase(q, 1);
		}
		
		// Deleting the k state
		for (int i = 0; i < nrOfStates; ++i)
		{
			if (transitionTable[i][k] != "")
			{
				transitionTable[i][k] = "";
				nrOfTransitions--;
			}
			if (transitionTable[k][i] != "")
			{
				transitionTable[k][i] = "";
				nrOfTransitions--;
			}
		}
	}

	isInGNFAFInalState = true;
}

void Automata::outputRegex(ofstream& fout)
{
	if (isInGNFAFInalState)
	{
		fout << transitionTable[initialState][acceptStates[0]];
	}
}

int main()
{
	// File Streams
	ifstream fin("input.in");
	ofstream fout("output.out");

	Automata A(fin);

	A.convertToRegex();
	
	A.outputRegex(fout);
	return 0;
}