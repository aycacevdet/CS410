#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <map>
#include <deque>

using namespace std;

class State {
public:
	string name;
	map<string, set<State*>> transition;

	State(string name) {
		this->name = name;
	}

	State* copy() {
		State* stateCopy = new State(this->name);
		for (auto iter = this->transition.begin(); iter != this->transition.end(); iter++) {
			stateCopy->transition[(*iter).first] = (*iter).second;
		}
		return stateCopy;
	}
};

class FiniteAutomata {
public:
	set<State*> states;
	set<string> alphabet;
	State* startState;
	set<State*> finalStates;

	FiniteAutomata() {
		this->alphabet = {};
		this->states = {};
		this->startState = 0;
		this->finalStates = {};
	}

	~FiniteAutomata() {
		for (auto state : states) {
			delete state;
		}
	}

	State* getStateByName(string stateName) {
		for (auto state : this->states)
		{
			if (state->name.compare(stateName) == 0) {
				return state;
			}
		}
		return 0;
	}

	FiniteAutomata* NFAtoDFA() {
		// instantiate new FiniteAutomata object for DFA
		FiniteAutomata* dfa = new FiniteAutomata();

		// initialize deque
		// deque will hold states to be processed
		deque<State*> que;

		// push copy of the startState to deque
		que.push_back(this->startState->copy());

		// iteration will continue until deque is empty
		while (!que.empty()) {

			// start processing the first state in deque
			State* state = que.front();

			// for each character in alphabet check reached states
			for (auto it = state->transition.begin(); it != state->transition.end(); it++)
			{
				set<State*> reachedStates = (*it).second;
				State* newState = createDFAState(reachedStates);

				if (newState != 0) {
					// check if newState is already in deque
					for (auto it2 = que.begin(); it2 != que.end(); it2++) {
						if ((*it2)->name == newState->name) {
							delete newState;
							newState = 0;
							break;
						}
					}
				}
				if (newState != 0) {
					// check if newState is already in dfa->states
					for (auto dfaState : dfa->states) {
						if (dfaState->name == newState->name) {
							delete newState;
							newState = 0;
							break;
						}
					}
				}
				// if newState is not in deque and dfa->states, push newState to deque
				if (newState != 0) {
					que.push_back(newState);
				}
			}
			// since state is processed, insert state to dfa->states
			dfa->states.insert(state);
			// check if state is final state
			for (auto finalState : this->finalStates)
			{
				int index = state->name.find(finalState->name);
				if (index != -1) {
					dfa->finalStates.insert(state);
					break;
				}
			}
			// check if state is start state
			if (state->name.compare(this->startState->name) == 0) {
				dfa->startState = state;
			}
			// pop processed state 
			que.pop_front();
		}
		dfa->alphabet = this->alphabet;
		dfa = dfa->arrangeStates(dfa);

		return dfa;
	}

	State* createDFAState(set<State*> reachedStates) {
		string name = "";
		set<State*> unionSet = {};

		// create name for new state
		for (auto reached : reachedStates) {
			name += reached->name;
		}
		if (name.compare("") == 0) {
			return 0;
		}

		// if name is not null create new state
		State* newState = new State(name);

		// for each character in alphabet create transitions of the new state
		for (auto alpha : alphabet) {
			for (auto reached : reachedStates) {
				set<State*> transitionStates = reached->transition[alpha];
				for (auto transitionState : transitionStates) {
					unionSet.insert(transitionState);
				}
			}
			// check if transition is empty for the character, assign transition if not empty
			if (!unionSet.empty()) {
				newState->transition[alpha] = unionSet;
			}
			unionSet = {};
		}
		return newState;
	}

	FiniteAutomata* arrangeStates(FiniteAutomata* fa) {
		// states were seperate in the transition map for set merging purposes, now concatenate the states
		for (auto state : fa->states) {
			string name = "";
			for (auto it = state->transition.begin(); it != state->transition.end(); it++) {
				string alpha = (*it).first;
				set<State*> reachedStates = (*it).second;
				for (auto reached : reachedStates) {
					name += reached->name;
				}
				// reset transition function
				state->transition[alpha] = {};
				// assign new corresponding state
				state->transition[alpha].insert(fa->getStateByName(name));
				name = "";
			}
		}
		return fa;
	}

	void display() {
		cout << "ALPHABET" << endl;
		for (auto alpha : this->alphabet) {
			cout << alpha << endl;
		}
		cout << "STATES" << endl;
		for (auto state : this->states) {
			cout << state->name << endl;
		}
		cout << "START" << endl;
		cout << this->startState->name << endl;

		cout << "FINAL" << endl;
		for (auto state : this->finalStates) {
			cout << state->name << endl;
		}
		cout << "TRANSITIONS" << endl;
		for (auto state : this->states) {
			for (auto it = state->transition.begin(); it != state->transition.end(); it++) {
				for (auto itt : (*it).second) {
					cout << state->name << " " << (*it).first << " " << (*itt).name << endl;
				}
			}
		}
		cout << "END" << endl;
	}
};


int main(int argc, char* argv[]) {

	string testfile;
	if (argc < 2) {
		cout << "Enter a test file name: " << endl;
		cin >> testfile;
	}
	else {
		testfile = argv[1];
	}
	ifstream file(testfile.c_str());
	if (file.is_open()) {
		cin.rdbuf(file.rdbuf());
		cout << "Reading the file..." << endl;
	}
	else {
		cout << "Error: cannot read the test file!" << endl;
		return -1;
	}
	cout << endl;

	string next = "";
	cin >> next;
	FiniteAutomata* fa = new FiniteAutomata();

	// read file and create the NFA
	if (next.compare("ALPHABET") == 0) {
		cin >> next;
		while (next.compare("STATES") != 0) {
			fa->alphabet.insert(next);
			cin >> next;
		}
	}
	if (next.compare("STATES") == 0) {
		cin >> next;
		while (next.compare("START") != 0) {
			State* state = new State(next);
			fa->states.insert(state);
			cin >> next;
		}
	}
	if (next.compare("START") == 0) {
		cin >> next;
		while (next.compare("FINAL") != 0) {
			fa->startState = fa->getStateByName(next);
			cin >> next;
		}
	}
	if (next.compare("FINAL") == 0) {
		cin >> next;
		while (next.compare("TRANSITIONS") != 0) {
			fa->finalStates.insert(fa->getStateByName(next));
			cin >> next;
		}
	}
	if (next.compare("TRANSITIONS") == 0) {
		string first = "";
		cin >> first;
		while (first.compare("END") != 0) {
			string alpha = "";
			string second = "";
			cin >> alpha;
			cin >> second;

			State* firstState = fa->getStateByName(first);
			State* secondState = fa->getStateByName(second);

			firstState->transition[alpha].insert(secondState);
			cin >> first;
		}
	}

	cout << "NFA is created." << endl;
	fa->display();
	cout << "-------------------------" << endl;
	cout << "Converting NFA to DFA..." << endl << endl;
	FiniteAutomata* dfa = fa->NFAtoDFA();
	cout << "DFA is created." << endl;
	dfa->display();

	delete fa;
	delete dfa;
}