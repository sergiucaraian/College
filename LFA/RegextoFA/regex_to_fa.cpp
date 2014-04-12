#include <fstream>
#include <string>
using namespace std;

class Automata
{
	char** transitionTable;
	int nrOfStates;
	int initialState;
	int finalState;
public:
	// Constructors
	Automata() { nrOfStates = 0; initialState = -1; finalState = -1; transitionTable = NULL; }
	Automata(int,int,int);
	Automata(string);
	Automata(const Automata&);
	
	// Setters and getters
	int getNrOfStates() { return nrOfStates; }
	int getInitialState() { return initialState; }
	int getFinalState() { return finalState; }
	char** getTransitionTable() { return transitionTable; }

	void setNrOfStates(int arg) { nrOfStates = arg; }
	void setInitialState(int arg) { initialState = arg; }
	void setFinalState(int arg) { finalState = arg; }

	// Other functions
	void shift(int);
	void addEmptyState();
	void addTransition(char value, int state1, int state2) { transitionTable[state1][state2] = value; }
	void output(ofstream&);
};


Automata::Automata(int nrOfStatesArg, int initialStateArg, int finalStateArg)
{
	nrOfStates = nrOfStatesArg;
	initialState = initialStateArg;
	finalState = finalStateArg;
	transitionTable = new char*[nrOfStates];
	for (int i = 0; i < nrOfStates; ++i)
		transitionTable[i] = new char[nrOfStates]();
}

Automata::Automata(const Automata& objectArg)
{
	nrOfStates = objectArg.nrOfStates;
	initialState = objectArg.initialState;
	finalState = objectArg.finalState;
	
	transitionTable = new char*[nrOfStates];
	for (int i = 0; i < nrOfStates; ++i)
		transitionTable[i] = new char[nrOfStates]();

	for (int i = 0; i < nrOfStates; ++i)
		for (int j = 0; j < nrOfStates; ++j)
			transitionTable[i][j] = objectArg.transitionTable[i][j];
}

void Automata::shift(int shiftArg)
{
	int newNrOfStates = nrOfStates + shiftArg;
	
	char** newTransitionTable = new char*[newNrOfStates];
	for (int i = 0; i < newNrOfStates; ++i)
		newTransitionTable[i] = new char[newNrOfStates]();
	
	for (int i = 0; i < nrOfStates; ++i)
	for (int j = 0; j < nrOfStates; ++j)
		newTransitionTable[i + shiftArg][j + shiftArg] = transitionTable[i][j];

	for (int i = 0; i < nrOfStates; ++i)
		delete[] transitionTable[i];

	transitionTable = newTransitionTable;
	nrOfStates = newNrOfStates;
	initialState = initialState + shiftArg;
	finalState = finalState + shiftArg;
}

void Automata::addEmptyState()
{
	char** newTransitionTable = new char*[nrOfStates + 1];
	for (int i = 0; i < nrOfStates + 1; ++i)
		newTransitionTable[i] = new char[nrOfStates+1]();

	for (int i = 0; i < nrOfStates; ++i)
	for (int j = 0; j < nrOfStates; ++j)
		newTransitionTable[i][j] = transitionTable[i][j];

	for (int i = 0; i < nrOfStates; ++i)
		delete[] transitionTable[i];

	transitionTable = newTransitionTable;
	nrOfStates = nrOfStates + 1;
}

Automata buildSimpleAutomata(char arg)
{
	Automata B(2, 0, 1);
	B.addTransition(arg, 0, 1);
	return B;
}

class NodeStackAutomata
{
	char value;
	Automata* automataPointer;
	NodeStackAutomata* nextNode;
public:
	NodeStackAutomata() { value = 0; automataPointer = NULL; nextNode = NULL; }
	void setValue(char arg) { value = arg; }
	char getValue() { return value; }
	NodeStackAutomata* getNextNode() { return nextNode; }
	void setNextNode(NodeStackAutomata* arg) { nextNode = arg; }
	Automata* getAutomataPointer() { return automataPointer; }
	void setAutomataPointer(Automata* automataPointerArg){ automataPointer = automataPointerArg; }
};

class StackAutomata
{
	NodeStackAutomata* head;
public:
	StackAutomata() { head = NULL; }
	StackAutomata(NodeStackAutomata*);
	void push(char);
	bool isEmpty() { if (head) return false; return true; }
	bool isOperation(char arg) { if (arg == 42 || arg == 40 || arg == 41 || arg == 124) return true; return false; }
	bool isState(char arg) { if (!isOperation(arg)) return true; return false; }
	NodeStackAutomata* getHead() { return head; }
	Automata* pop();
};

StackAutomata::StackAutomata(NodeStackAutomata* nodeArg)
{
	head = nodeArg;
}

void StackAutomata::push(char arg)
{
	if (!head)
	{
		head = new NodeStackAutomata;
		head->setValue(arg);
		head->setAutomataPointer(NULL);
		head->setNextNode(NULL);
	}
	else
	{
		NodeStackAutomata* auxNode = new NodeStackAutomata;
		auxNode->setValue(arg);
		auxNode->setAutomataPointer(NULL);
		auxNode->setNextNode(head);
		head = auxNode;
	}
}

Automata* star(Automata* automataArg)
{
	automataArg->shift(1);
	automataArg->addEmptyState();

	automataArg->addTransition(-1, automataArg->getFinalState(), automataArg->getInitialState());
	automataArg->addTransition(-1, 0, automataArg->getInitialState());
	automataArg->addTransition(-1, automataArg->getFinalState(), automataArg->getNrOfStates()-1);
	automataArg->addTransition(-1, 0, automataArg->getNrOfStates()-1);

	automataArg->setInitialState(0);
	automataArg->setFinalState(automataArg->getNrOfStates() - 1);
	
	return automataArg;
}

Automata* unionAutomata(Automata* automataArg1, Automata* automataArg2)
{
	automataArg1->shift(1);
	automataArg2->shift(automataArg1->getNrOfStates());

	Automata* newAutomata = new Automata(*automataArg2);
	char** auxTransitionTable = automataArg1->getTransitionTable();
	for (int i = 0; i < automataArg1->getNrOfStates(); ++i)
	for (int j = 0; j < automataArg1->getNrOfStates(); ++j)
		newAutomata->addTransition(auxTransitionTable[i][j], i, j);
	newAutomata->setInitialState(0);

	newAutomata->addTransition(-1, 0, automataArg1->getInitialState());
	newAutomata->addTransition(-1, 0, automataArg2->getInitialState());
	newAutomata->setInitialState(0);

	newAutomata->addEmptyState();
	newAutomata->setFinalState(newAutomata->getNrOfStates() - 1);
	newAutomata->addTransition(-1, automataArg1->getFinalState(), newAutomata->getFinalState());
	newAutomata->addTransition(-1, automataArg2->getFinalState(), newAutomata->getFinalState());

	return newAutomata;
}

Automata* concatenation(Automata* automataArg1, Automata* automataArg2)
{
	automataArg2->shift(automataArg1->getNrOfStates() - 1);

	Automata* newAutomata = new Automata(*automataArg2);
	char** auxTransitionTable = automataArg1->getTransitionTable();
	for (int i = 0; i < automataArg1->getNrOfStates(); ++i)
	for (int j = 0; j < automataArg1->getNrOfStates(); ++j)
		newAutomata->addTransition(auxTransitionTable[i][j], i, j);
	newAutomata->setInitialState(0);
	return newAutomata;
}

Automata* StackAutomata::pop()
{
	if (head->getValue() == ')')
	{
		int counter = 0;
		StackAutomata restOfStack;
		NodeStackAutomata* currentNode = head->getNextNode();
		while (counter != -1)
		{
			if (currentNode->getValue() == '(')
				counter--;
			else if (currentNode->getValue() == ')')
				counter++;
			if (counter != -1)
			{
				restOfStack.push(currentNode->getValue());
				currentNode = currentNode->getNextNode();
			}
		}
		
		// Reversing restofStack
		StackAutomata auxRestOfStack;
		NodeStackAutomata* auxPointer = restOfStack.getHead();
		while (auxPointer)
		{
			auxRestOfStack.push(auxPointer->getValue());
			auxPointer = auxPointer->getNextNode();
		}

		Automata* convertedAutomata = new Automata;
		convertedAutomata = auxRestOfStack.pop();

		head = currentNode->getNextNode();
		NodeStackAutomata* auxNode = new NodeStackAutomata;
		auxNode->setValue(-2);
		auxNode->setAutomataPointer(convertedAutomata);
		auxNode->setNextNode(head);
		head = auxNode;
		StackAutomata newStack(head);
		return newStack.pop();
	}

	else if (head->getValue() == '*')
	{
		if (isState(head->getNextNode()->getValue()))
		{
			Automata auxAutomata = buildSimpleAutomata(head->getNextNode()->getValue());
			Automata* convertedAutomata = new Automata(auxAutomata);
			Automata* newAutomata = star(convertedAutomata);
			NodeStackAutomata* auxNode = new NodeStackAutomata;
			auxNode->setValue(-2);
			auxNode->setAutomataPointer(newAutomata);
			auxNode->setNextNode(head->getNextNode()->getNextNode());
			head = auxNode;
			StackAutomata newStack(head);
			return newStack.pop();
		}
		else if (head->getNextNode()->getValue() == ')')
		{
			int counter = 0;
			StackAutomata restOfStack;
			NodeStackAutomata* currentNode = head->getNextNode()->getNextNode();
			while (counter != -1)
			{
				if (currentNode->getValue() == '(')
					counter--;
				else if (currentNode->getValue() == ')')
					counter++;
				if (counter != -1)
				{
					restOfStack.push(currentNode->getValue());
					currentNode = currentNode->getNextNode();
				}
			}
			
			// Reversing restofStack
			StackAutomata auxRestOfStack;
			NodeStackAutomata* auxPointer = restOfStack.getHead();
			while (auxPointer)
			{
				auxRestOfStack.push(auxPointer->getValue());
				auxPointer = auxPointer->getNextNode();
			}

			Automata* convertedAutomata = new Automata;
			convertedAutomata = restOfStack.pop();
			Automata* newAutomata = star(convertedAutomata);
			
			NodeStackAutomata* auxNode = new NodeStackAutomata;
			auxNode->setValue(-2);
			auxNode->setAutomataPointer(newAutomata);
			auxNode->setNextNode(currentNode->getNextNode());
			head = auxNode;
			StackAutomata newStack(head);
			return newStack.pop();
		}
	}
	else
	{
		Automata headAutomata;
		if (head->getValue()!=-2)
			headAutomata = buildSimpleAutomata(head->getValue());
		else headAutomata = *(head->getAutomataPointer());
		Automata* convertedAutomata = new Automata;
		if (head->getNextNode()!=NULL && head->getNextNode()->getValue() == '|')
		{
			StackAutomata restOfStack;
			
			NodeStackAutomata* currentNode = head->getNextNode()->getNextNode();
			int counter = 0;
			while (currentNode && !(currentNode->getValue()=='|' && counter==0) && counter != -1)
			{
				if (currentNode->getValue() == '(')
					counter--;
				else if (currentNode->getValue() == ')')
					counter++;
				if (counter != -1)
				{
					restOfStack.push(currentNode->getValue());
					currentNode = currentNode->getNextNode();
				}
			}

			StackAutomata auxRestOfStack;
			NodeStackAutomata* auxPointer = restOfStack.getHead();
			while (auxPointer)
			{
				auxRestOfStack.push(auxPointer->getValue());
				auxPointer = auxPointer->getNextNode();
			}

			convertedAutomata = auxRestOfStack.pop();
			Automata* newAutomata = unionAutomata(convertedAutomata, &headAutomata);
			
			NodeStackAutomata* auxNode = new NodeStackAutomata;
			auxNode->setValue(-2);
			auxNode->setAutomataPointer(newAutomata);
			auxNode->setNextNode(currentNode);
			head = auxNode;
			StackAutomata newStack(head);
			return newStack.pop();
		}
		else
		{
			if (head->getNextNode() == NULL)
			{
				Automata auxAutomata;
				if (head->getValue()!=-2)
					auxAutomata = buildSimpleAutomata(head->getValue());
				else auxAutomata = *(head->getAutomataPointer());
				Automata* newAutomata = new Automata(auxAutomata);
				return newAutomata;
			}
			else
			{
				NodeStackAutomata* currentNode = head->getNextNode();
				StackAutomata restOfStack;
				int counter = 0;
				while (currentNode && !(currentNode->getValue() == '|' && counter == 0) && counter != -1)
				{
					if (currentNode->getValue() == '(')
						counter--;
					else if (currentNode->getValue() == ')')
						counter++;
					if (counter != -1)
					{
						restOfStack.push(currentNode->getValue());
						currentNode = currentNode->getNextNode();
					}
				}
				
				StackAutomata auxRestOfStack;
				NodeStackAutomata* auxPointer = restOfStack.getHead();
				while (auxPointer)
				{
					auxRestOfStack.push(auxPointer->getValue());
					auxPointer = auxPointer->getNextNode();
				}
				
				convertedAutomata = auxRestOfStack.pop();
				Automata* newAutomata = concatenation(convertedAutomata, &headAutomata);
				
				NodeStackAutomata* auxNode = new NodeStackAutomata;
				auxNode->setValue(-2);
				auxNode->setAutomataPointer(newAutomata);
				auxNode->setNextNode(currentNode);
				head = auxNode;
				StackAutomata newStack(head);
				return newStack.pop();
			}
		}
	}
}

Automata::Automata(string regexArg)
{
	StackAutomata parserStack;
	for (int i = 0; i < regexArg.length(); ++i)
	{
		parserStack.push(regexArg[i]);
	}

	Automata* B = new Automata();
	B = parserStack.pop();

	// Copying the new automata to the current one
	nrOfStates = B->getNrOfStates();
	initialState = B->getInitialState();
	finalState = B->getFinalState();
	
	char**auxTransitionTable = B->getTransitionTable();
	transitionTable = new char*[nrOfStates];
	for (int i = 0; i < nrOfStates; ++i)
		transitionTable[i] = new char[nrOfStates];
	for (int i = 0; i < nrOfStates; ++i)
	for (int j = 0; j < nrOfStates; ++j)
		transitionTable[i][j] = auxTransitionTable[i][j];
}

void Automata::output(ofstream& fout)
{
	fout << "Number of States: " << nrOfStates << '\n';
	fout << "Start state: " << initialState << '\n';
	fout << "Final state: " << finalState << '\n';

	for (int i = 0; i < nrOfStates; ++i)
	for (int j = 0; j < nrOfStates; ++j)
	{
		if (transitionTable[i][j] == -1)
			fout << i << " -> E -> " << j << '\n';
		else if (transitionTable[i][j] != 0)
			fout << i << " -> " << transitionTable[i][j] << " -> " << j << '\n';
	}
}

int main()
{
	ifstream fin("input.in");
	ofstream fout("output.out");
	
	// Reading the regex
	string regexInput;
	fin >> regexInput;
	
	// Building automata from regex
	Automata A(regexInput);

	// Printing the automata
	A.output(fout);

	return 0;
}