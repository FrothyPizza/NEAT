#pragma once

#include <cmath>
#include <vector>


#include "Connection.h"

class Connection;

class Node {

public:

	// the id number of this node
	int number;

	// the layer this node is in
	// 0 is input
	int layer;

	// the sum of the inputs
	float sumOfInputs;

	// the output value of this node
	float outputValue;

	// vector of connection pointers
	std::vector<Connection*> connections;

	Node(int number);


	
	// applys activation function to value
	// for each weight going out of it, multiply this value by the weight and add that to the connected node
	void activate();

	bool isConnectedTo(Node* other);


	static float sigmoid(float x) {
		return 1 / (1 + static_cast<float>(pow(2.71828f, -4.9 * x)));
	}
};

