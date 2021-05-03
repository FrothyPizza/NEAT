#include "Node.h"

Node::Node(int number) : number{ number }, layer{ 0 }, sumOfInputs{ 0 }, outputValue{ 0 } {

}

void Node::activate() {
	if(layer != 0) outputValue = sigmoid(sumOfInputs);

	for (unsigned int i = 0; i < connections.size(); ++i) {
		if(connections.at(i)->enabled)
			connections.at(i)->toNode->sumOfInputs += outputValue * connections.at(i)->weight;
	}

}

bool Node::isConnectedTo(Node* other) {

	for (size_t i = 0; i < connections.size(); ++i) {
		if (connections.at(i)->toNode == other) {
			return true;
		}
	}

	return false;
}
