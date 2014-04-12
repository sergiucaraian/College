#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

//
//  STATE
//

class State
{
	string stateValue;
public:
	State() { stateValue = ""; };
	State(string stateValueArg) { stateValue = stateValueArg; };
	void setValue(string stateValueArg) { stateValue = stateValueArg; };
	string getValue() { return stateValue; };
	bool operator== (State);
	void removeDuplicateStates();
};

bool State::operator==(State stateArg)
{
	if (stateValue == stateArg.getValue())
		return true;
	else return false;
}

void State::removeDuplicateStates()
{
	int index = 0;
	vector<string> stateVector;

	while (stateValue.find_first_of(",", index) != string::npos)
	{
		string state = stateValue.substr(index, stateValue.find_first_of(",", index) - index);
		stateVector.push_back(state);
		index = index + state.length() + 1;
	}

	string state = stateValue.substr(index, state.length() - index);
	stateVector.push_back(state);

	for (int i = 0; i < stateVector.size()-1; ++i)
	for (int j = i + 1; j < stateVector.size(); ++j)
	if (stateVector[i] == stateVector[j])
		stateVector.erase(stateVector.begin() + j);
	
	stateValue = stateVector[0];
	for (int i = 1; i < stateVector.size(); ++i)
		stateValue = stateValue + "," + stateVector[i];
}
//
// AUTOMATA
//

class Automata
{
	// 5 - tuple
	State* Q;
	string* E;
	State* q0;
	State* F;

	// other attributes
	int nrOfStates;
	int nrOfLetters;
	int nrOfTransitions;
	int nrOfAcceptStates;

	// transition matrix
	State** transitionTable;
	int getIndex(State);
	int getIndex(string);
	bool isAcceptState(State);
	bool isState(char);
	bool isOperator(char);
public:
	Automata();
	Automata(ifstream&);
	Automata(string);
	void output(ofstream&);
	void convertToDFA();
};

int Automata::getIndex(State stateArg)
{
	for (int i = 0; i<nrOfStates; ++i)
	{
		if (Q[i].getValue() == stateArg.getValue())
			return i;
	}
	return -1;
}

int Automata::getIndex(string letterArg)
{
	for (int i = 0; i<nrOfLetters; ++i)
	{
		if (E[i] == letterArg)
			return i;
	}
	return -1;
}

bool Automata::isAcceptState(State stateArg)
{
	int index = 0;
	vector<string> stateVector;

	while (stateArg.getValue().find_first_of(",", index) != string::npos)
	{
		string state = stateArg.getValue().substr(index, stateArg.getValue().find_first_of(",", index) - index);
		for (int i = 0; i < nrOfAcceptStates; ++i)
			if (state == F[i].getValue())
				return true;
		index = index + state.length() + 1;
	}
	string state = stateArg.getValue().substr(index, stateArg.getValue().length()-index);
	for (int i = 0; i < nrOfAcceptStates; ++i)
		if (state == F[i].getValue())
			return true;
	return false;
}

Automata::Automata()
{
	// Setting pointers to NULL
	Q = NULL;
	E = NULL;
	q0 = NULL;
	F = NULL;
	transitionTable = NULL;
}

Automata::Automata(ifstream& fin)
{
	// Reading states
	fin >> nrOfStates;
	Q = new State[nrOfStates];
	for (int i = 0; i<nrOfStates; ++i)
	{
		string buffer;
		fin >> buffer;
		Q[i].setValue(buffer);
	}

	// Reading the alphabet
	fin >> nrOfLetters;
	E = new string[nrOfLetters];
	for (int i = 0; i<nrOfLetters; ++i)
	{
		fin >> E[i];
	}

	// Reading the transitions
	fin >> nrOfTransitions;
	transitionTable = new State*[nrOfStates];
	for (int i = 0; i<nrOfStates; ++i)
		transitionTable[i] = new State[nrOfLetters];
	string stateBuffer1, stateBuffer2, letterBuffer;
	for (int i = 0; i<nrOfTransitions; ++i)
	{
		fin >> stateBuffer1 >> letterBuffer >> stateBuffer2;
		State state1(stateBuffer1), state2(stateBuffer2);
		int index1 = getIndex(state1);
		int index2 = getIndex(letterBuffer);
		if (transitionTable[index1][index2].getValue() != "")
		{
			stateBuffer1 = transitionTable[index1][index2].getValue();
			stateBuffer1 = stateBuffer1 + "," + stateBuffer2;
			transitionTable[index1][index2].setValue(stateBuffer1);
		}
		else transitionTable[index1][index2] = state2;
	}

	// Reading the start state
	string buffer;
	fin >> buffer;
	q0 = new State(buffer);

	// Reading the accept states
	fin >> nrOfAcceptStates;
	F = new State[nrOfAcceptStates];
	for (int i = 0; i<nrOfAcceptStates; ++i)
	{
		string buffer;
		fin >> buffer;
		F[i].setValue(buffer);
	}
}

void Automata::output(ofstream& fout)
{
	fout << "STATES:\n";
	for (int i = 0; i<nrOfStates; ++i)
		fout << Q[i].getValue() << " ";
	fout << '\n';

	fout << "ALPHABET:\n";
	for (int i = 0; i<nrOfLetters; ++i)
		fout << E[i] << " ";
	fout << '\n';

	fout << "TRANSITIONS:\n";
	for (int i = 0; i<nrOfStates; ++i)
	for (int j = 0; j<nrOfLetters; ++j)
	{
		if (transitionTable[i][j].getValue() != "")
			fout << Q[i].getValue() << " " << E[j] << " " << transitionTable[i][j].getValue() << '\n';
	}

	fout << "START STATE:\n";
	fout << q0->getValue() << '\n';

	fout << "ACCEPT STATES:\n";
	for (int i = 0; i<nrOfAcceptStates; ++i)
		fout << F[i].getValue() << " ";
	fout << '\n';
}

void Automata::convertToDFA()
{
	vector < vector<State> > conversionTable(2 * nrOfStates, vector<State>(nrOfLetters));
	vector <State> newStates;
	vector <State> acceptStates;

	newStates.push_back(*q0);
	if (isAcceptState(*q0))
		acceptStates.push_back(*q0);
	int currentStateIndex = 0;

	while (currentStateIndex < newStates.size())
	{
		State currentState = newStates[currentStateIndex];

		for (int i = 0; i < nrOfLetters; ++i)
		{
			string newStateValue = "";
			State stateBuffer = currentState;

			if (stateBuffer.getValue().find_first_of(",") == string::npos && stateBuffer.getValue() != "")
			{
				// New state is made of only one original state
				conversionTable[currentStateIndex][i] = transitionTable[getIndex(stateBuffer)][i];
				bool found = false;
				for (int j = 0; j < newStates.size() && found == false; ++j)
				if (newStates[j].getValue() == transitionTable[getIndex(stateBuffer)][i].getValue())
					found = true;
				if (!found && transitionTable[getIndex(stateBuffer)][i].getValue() != "")
				{
					newStates.push_back(transitionTable[getIndex(stateBuffer)][i]);
					if (isAcceptState(transitionTable[getIndex(stateBuffer)][i]))
						acceptStates.push_back(transitionTable[getIndex(stateBuffer)][i]);
				}
			}

			else
			{
				// More then one original state so we need to tokenize the state
				while (stateBuffer.getValue().find_first_of(",") != string::npos)
				{
					string buffer;
					buffer = stateBuffer.getValue().substr(0, stateBuffer.getValue().find_first_of(","));
					State auxBuffer(buffer);
					int auxIndex = getIndex(auxBuffer);

					if (transitionTable[auxIndex][i].getValue() != "")
					{
						if (newStateValue == "")
							newStateValue = newStateValue + transitionTable[auxIndex][i].getValue();
						else newStateValue = newStateValue + "," + transitionTable[auxIndex][i].getValue();
					}

					buffer = stateBuffer.getValue();
					buffer.erase(0, buffer.find_first_of(",") + 1);
					stateBuffer.setValue(buffer);
				}

				int auxIndex = getIndex(stateBuffer);
				if (transitionTable[auxIndex][i].getValue() != "")
				{
					if (newStateValue == "")
						newStateValue = newStateValue + transitionTable[auxIndex][i].getValue();
					else newStateValue = newStateValue + "," + transitionTable[auxIndex][i].getValue();
				}

				// Removing duplicate states
				State auxBuff(newStateValue);
				auxBuff.removeDuplicateStates();
				newStateValue = auxBuff.getValue();

				conversionTable[currentStateIndex][i].setValue(newStateValue);
				bool found = false;
				for (int j = 0; j < newStates.size() && found == false; ++j)
				{
					if (newStates[j].getValue() == newStateValue)
						found = true;
				}
				State stateAuxBuffer(newStateValue);
				if (!found && newStateValue != "")
				{
					newStates.push_back(stateAuxBuffer);
					if (isAcceptState(stateAuxBuffer))
						acceptStates.push_back(stateAuxBuffer);
				}
			}
		}

		//Moving to the next new state
		currentStateIndex++;
	}
		// Setting the old states to the new ones
		delete[] Q;
		Q = new State[newStates.size()];
		int backupNrOfStates = nrOfStates;
		nrOfStates = newStates.size();
		for (int i = 0; i < nrOfStates; ++i)
			Q[i] = newStates[i];


		// Setting the old table to the new one
		for (int i = 0; i < backupNrOfStates; ++i)
			delete[] transitionTable[i];
		delete[] transitionTable;

		transitionTable = new State*[nrOfStates];
		for (int i = 0; i < nrOfStates; ++i)
			transitionTable[i] = new State[nrOfLetters];
		for (int i = 0; i < nrOfStates; ++i)
		for (int j = 0; j < nrOfLetters; ++j)
		if (conversionTable[i][j].getValue() != "")
			transitionTable[i][j] = conversionTable[i][j];

		// Setting the old access states to the new ones
		delete[] F;
		F = new State[acceptStates.size()];
		nrOfAcceptStates = acceptStates.size();
		for (int i = 0; i < nrOfAcceptStates; ++i)
			F[i] = acceptStates[i];
}
 

int main()
{
	// declaring streams
	ifstream fin("input.in");
	ofstream fout("output.out");

	// declaring automata
	Automata A(fin);

	// converting the automata to DFA
	A.convertToDFA();

	// printing automata
	A.output(fout);

	return 0;
}
