#pragma once

#include "Node.h"

class Node;



class Connection {
public:

	Node* fromNode;
	Node* toNode;

	bool enabled{ true };
	float weight;

	// for comparing genomes
	int innovationNumber;

	Connection(Node* from, Node* to, float w, int inno);

	// mutates the weight
	// 10% chance of completeley changing it
	// otherwise slightly change it
	void mutateWeight();

	
};

