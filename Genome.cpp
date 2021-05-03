#include "Genome.h"

// stack overflow to the rescue
float random(float Min, float Max) {
	return ((float(rand()) / float(RAND_MAX)) * (Max - Min)) + Min;
}


void Genome::orderNetwork() {

	std::vector<Node> net;
	net.reserve(nodes.size());

	for (int l = 0; l < layers; ++l) {             // for each layer
		for (size_t n = 0; n < nodes.size(); ++n) {// for each node
			if(nodes.at(n).layer == l)             // if the node is in the layer
				net.push_back(nodes.at(n));        // add it to the new network
		}
	}

	// assign the ordered network to orderedNetork
	orderedNetwork = net;
}

Genome::Genome(int inputs, int outputs) : inputs{ inputs }, outputs{ outputs }, nextNode{ 0 }, nextConnection{ 0 } {

	// since vector push_back invalidated all pointers to it if it needs to reallocate space, an arbitrary amount has to be reserved
	// this means that the genome can't have more than 100 hidden nodes (really unlikely to happen)
	nodes.reserve(inputs + outputs + 100);

	connections.reserve(inputs * outputs + 1000);
	
	// add inputs
	for (int i = 0; i < inputs; ++i) {
		nodes.push_back(Node{ nextNode });
		nextNode++;
		nodes.at(i).layer = 0;
	}

	// bias node
	nodes.push_back(Node{ nextNode });
	nodes.at(nextNode).layer = 0;
	biasNode = nextNode;
	nextNode++;

	// add outputs
	for (int i = 0; i < outputs; ++i) {
		nodes.push_back(Node{ nextNode });
		nodes.at(i + inputs + 1).layer = 1; // +1 for bias
		nextNode++;
	}

	
	// connect all inputs to outputs
	int innovations = 0;
	for (int i = 0; i < inputs + 1; ++i) { // +1 for bias
		for (int j = 0; j < outputs; ++j) {
			connections.push_back(Connection{ &nodes.at(i), &nodes.at(inputs + 1 + j), random(-1, 1), innovations }); //inputs+1 for bias
			if (innovations == biasNode) connections.at(biasNode).weight = 0;
			innovations++;
		}
	}
	

	// connect the nodes
	connectNodes();

	// order the network for feeding forward
	orderNetwork();
}

Genome::Genome(Genome& other) {
	// using this-> here for clarification
	this->inputs = other.inputs;
	this->outputs = other.outputs;

	this->layers = other.layers;



	// make this nodes and connections equal to other nodes
	// note: this also copies the pointers, so those will have to be redone
	this->nodes = other.nodes;
	this->connections = other.connections;

	this->nodes.reserve(other.inputs + other.outputs + 100); // reserve arbitrary amount based on i/o
	this->connections.reserve(other.inputs * other.outputs + 1000); // reserve arbitrary amount based on i/o

	for (size_t i = 0; i < this->nodes.size(); ++i) { // for each node
		for (size_t j = 0; j < this->nodes.at(i).connections.size(); ++j) { // for each connection that node has the pointer to
			// the connection pointer at index j of node i is assigned to the correct pointer
			ptrdiff_t connectionIndex = other.nodes.at(i).connections.at(j) - &other.connections[0];
			this->nodes.at(i).connections.at(j) = &this->connections.at(connectionIndex);
		}
	}

	for (size_t i = 0; i < this->connections.size(); ++i) { // for each connection
		// assign this from and to node to the correct pointer
		this->connections.at(i).fromNode = &this->nodes.at(other.connections.at(i).fromNode->number);
		this->connections.at(i).toNode = &this->nodes.at(other.connections.at(i).toNode->number);

	}

	connectNodes();

	orderNetwork();

}



void Genome::mutate(std::vector<ConnectionHistory>& connectionHistory) {



	static int c{ 0 };
	int r = rand() % 100;
	if (r <= 80) { // 80% chance to mutate the weights
		for (size_t i = 0; i < connections.size(); ++i) {
			if (rand() % 2 == 1) { // 50% chance for each weight
				connections.at(i).mutateWeight();
			}
		}
	}

	r = rand() % 100;
	if (r <= 5 && connections.size() > 0) { // 5% chance to add node 
		addNode(connectionHistory);
	}

	r = rand() % 100;
	if (r <= 8) { // 8% chance to add connection
		addConnection(connectionHistory);
	}



	orderNetwork();
}

std::vector<float> Genome::getOutputs(std::vector<float> inputs) {

	if (inputs.size() != this->inputs) std::cout << "amount of inputs not the same as neural network inputs";

	for (int i = 0; i < this->inputs; ++i) {
		orderedNetwork.at(i).outputValue = inputs.at(i);
	}

	// bias
	orderedNetwork.at(biasNode).outputValue = 1;

	for (size_t i = 0; i < orderedNetwork.size() - outputs; ++i) {
		orderedNetwork.at(i).activate();
	}

	std::vector<float> out;
	out.reserve(outputs);

	for (int i = 0; i < outputs; ++i) {
		float val = Node::sigmoid(nodes.at(nodes.size() - i - 1).sumOfInputs);
		out.push_back(val);
	}

	// reset nodes
	for (size_t i = 0; i < nodes.size(); ++i) {
		orderedNetwork.at(i).outputValue = 0;
		orderedNetwork.at(i).sumOfInputs = 0;
	}

	return out;
}



void Genome::addNode(std::vector<ConnectionHistory>& innovationHistory) {

	// choose a random connection to be split
	int connectionToBeSplit = rand() % connections.size();

	// dont add a node connecting from the bias node
	while (connections.at(connectionToBeSplit).fromNode->number == biasNode) {
		connectionToBeSplit = rand() % connections.size();
	}

	// disable the connection that is split
	connections.at(connectionToBeSplit).enabled = false;

	// add a new node with the correct number and advance the next node counter
	int nodeNo = nextNode; nextNode++;
	nodes.push_back(Node{ nodeNo });

	// create new connection between from node and new node with weight of one
	int innoNum = getInnovationNumber(innovationHistory, *connections.at(connectionToBeSplit).fromNode, nodes.at(nodes.size() - 1)); // TODO keep track of innovations
	connections.push_back(Connection{ connections.at(connectionToBeSplit).fromNode, &nodes.at(nodes.size() - 1), 1, innoNum });

	// create new connection between new node and to node with weight of the split node
	innoNum = getInnovationNumber(innovationHistory, nodes.at(nodes.size() - 1), *connections.at(connectionToBeSplit).toNode); // TODO keep track of innovations
	connections.push_back(Connection{ &nodes.at(nodes.size() - 1), connections.at(connectionToBeSplit).toNode, connections.at(connectionToBeSplit).weight, innoNum });

	// connect the bias node to it with a weight of 0
	innoNum = getInnovationNumber(innovationHistory, nodes.at(biasNode), nodes.at(nodeNo)); // TODO keep track of innovations
	connections.push_back(Connection{ &nodes.at(biasNode), &nodes.at(nodeNo), 0, innoNum });


	// set new node layer to the correct number
	nodes.at(nodes.size() - 1).layer = connections.at(connectionToBeSplit).fromNode->layer + 1;


	int newNodeLayer = nodes.at(nodes.size() - 1).layer;
	if (newNodeLayer == connections.at(connectionToBeSplit).toNode->layer) {
		// increase layer of each node higher than the new node by one
		
		for (size_t i = 0; i < nodes.size() - 1; ++i) { // don't do this for the current node
			if (newNodeLayer <= nodes.at(i).layer)
				nodes.at(i).layer++;
		}
		layers++;	
	}
	
	// correctly connect all of the nodes
	connectNodes();
	

}

void Genome::addConnection(std::vector<ConnectionHistory>& innovationHistory) {
	
	std::vector<std::pair<int, int>> possibleConnections;
	possibleConnections.reserve(10); // reserve some arbitrary amount

	for (size_t i = 0; i < nodes.size(); ++i) {    // for each connection
		for (size_t j = 0; j < nodes.size(); ++j) {// for each connection
			// if the layer of node i is less than node j
			if (nodes.at(i).layer < nodes.at(j).layer) {
				// then if they aren't connected, add them to the possible connections
				if (!nodes.at(i).isConnectedTo(&nodes.at(j))) { 
					// add them to the possible connections
					possibleConnections.push_back(std::pair<int, int>{ nodes.at(i).number, nodes.at(j).number });
				}
			}
		}
	}

	// if there are no possible connections to be added, return
	if (possibleConnections.size() == 0) {
		return;
	}

	// select random possible connection
	int r = rand() % possibleConnections.size();

	int inno = getInnovationNumber(innovationHistory, nodes.at(possibleConnections.at(r).first), nodes.at(possibleConnections.at(r).second)); // TODO keep track of innovations

	// add the new connection
	connections.push_back(Connection{ &nodes.at(possibleConnections.at(r).first),
									  &nodes.at(possibleConnections.at(r).second),
									  random(-1, 1), inno });


	//int coIndex = connections.size() - 1;
	//nodes.at(possibleConnections.at(r).first).connections.push_back(&connections.at(coIndex));

	// connect the nodes
	connectNodes();
}

int Genome::getInnovationNumber(std::vector<ConnectionHistory>& innovationHistory, Node& from, Node& to) {
	bool newInnovation{ true };

	// this innovation number is the newest connection
	int innovationNo = nextConnection;

	for (size_t i = 0; i < innovationHistory.size(); ++i) { // for each connection history
		
		// if the innovation histroy has this (it isn't new), then newInnovation = false
		// and return the innovation number
		if (innovationHistory.at(i).sameAs(*this, from, to)) {
			newInnovation = false;
			return innovationHistory.at(i).innovationNo;
		}

	}

	// if it is a new innovation
	if (newInnovation) {
		// temporary vector to store the innos
		std::vector<int> innovations;
		innovations.reserve(connections.size());

		// for each connection, add it to the innovation
		for (size_t i = 0; i < connections.size(); ++i) {
			innovations.push_back(connections.at(i).innovationNumber);
		}

		// add this innovation list to the innovationHistory
		innovationHistory.push_back(ConnectionHistory{ innovations, from.number, to.number, innovationNo });

		// increment the value of this next connection so that it can keep track of the value
		nextConnection++;
	}

	return innovationNo;
}

void Genome::connectNodes() {
	
	// completely clear all of the nodes connections
	for (size_t i = 0; i < nodes.size(); ++i) {
		//nodes.at(i).connections.erase(nodes.at(i).connections.begin(), nodes.at(i).connections.end());
		nodes.at(i).connections.clear();
	}

	for (size_t i = 0; i < connections.size(); ++i) {
		connections.at(i).fromNode->connections.push_back(&connections.at(i));
	}


}



// definitely extremely efficient and commented very well
void Genome::draw(sf::RenderWindow& window, const sf::Vector2f& startpos, const sf::Vector2f& endpos, float scale) {

	float distance = endpos.x - startpos.x;
	float ydist = endpos.y - startpos.y;
	float center = ydist / 2;

	float s = 5.f * scale;

	static sf::Line line{ sf::Vector2f{0, 0}, sf::Vector2f{100, 100}, sf::Color{255,0,0}, s / 6 };
	static sf::CircleShape circle{s / 4, 256};
	circle.setOrigin(sf::Vector2f(circle.getRadius(), circle.getRadius()));

	circle.setRadius(s / 3.f);
	circle.setFillColor(sf::Color::Black);


	std::vector<std::vector<Node>> layeredNodes;
	layeredNodes.reserve(layers);

	for (int l = 0; l < layers; ++l) { // for each layer 
		layeredNodes.push_back(std::vector<Node>{});
		layeredNodes.at(l).reserve(10);

		for (size_t i = 0; i < orderedNetwork.size(); ++i) { // for each node in that layer
			if (orderedNetwork.at(i).layer == l) {
				layeredNodes.at(l).push_back(orderedNetwork.at(i));
			}

		}
	}

	std::vector<sf::Vector2f> nodePositions;
	std::vector<int> nodeNumbers;
	nodeNumbers.reserve(nodes.size());
	nodePositions.reserve(nodes.size());

	for (int l = 0; l < layers; ++l) {
		for (size_t i = 0; i < layeredNodes.at(l).size(); ++i) {
			int ns = layeredNodes.at(l).size();
			
			//nodePositions.push_back(sf::Vector2f{ l * s * 10, (float)i * s * 2.f + i * s / 2.f } + startpos);
			//nodePositions.push_back(sf::Vector2f{ (distance / (layers - 1)) * l, (float)i * s * 2.f + i * s + l * s } +startpos);
			float margin = ydist / layeredNodes.at(l).size() / 6 + l * layeredNodes.at(l).size();
			nodePositions.push_back(sf::Vector2f{ (distance / (layers - 1)) * l, ((ydist-margin) / (ns)) * i + margin * 2 } +startpos);

			//nodePositions.push_back(sf::Vector2f{ (distance / (layers - 1)) * l, center + l*s/2 - ns * s + (i * s * (5.f / (float)ns))*2 - ns * s} +startpos);

			nodeNumbers.push_back(layeredNodes.at(l).at(i).number);
		}

	}


	// draw connections
	for (size_t i = 0; i < connections.size(); ++i) {


		sf::Vector2f start{ nodePositions.at(connections.at(i).fromNode->number) };
		sf::Vector2f end{ nodePositions.at(connections.at(i).toNode->number) };

		sf::Color color{};
		// blue for negative weight
		// red for positive
		// alpha representing distance from 0 (1 is solid, 0 is transparent)
		if (connections.at(i).weight > 0) {
			//color = sf::Color{ 255, 0, 0 ,  sf::Uint8(connections.at(i).weight * 255 + 20) }; // +20 so it's still visible 
			color = sf::Color{ 255, 0, 0 }; //,  sf::Uint8(connections.at(i).weight * 255 + 10) };
		}
		else {
			//color = sf::Color{ 0, 0, 255 , sf::Uint8(connections.at(i).weight * 255 + 20) }; // +20 so it's still visible 
			color = sf::Color{ 0, 0, 255 };// sf::Uint8(connections.at(i).weight * 255 + 10) };

		}
		

		line = sf::Line{ start, end, color, s / 8 };
		
		if (connections.at(i).enabled) {
			window.draw(line);
		}

	}


	for (size_t i = 0; i < nodePositions.size(); ++i) {
		circle.setPosition(nodePositions.at(i));
		if (i == inputs)circle.setFillColor(sf::Color{ 200, 200, 200 });
		else { circle.setFillColor(sf::Color::Black); }
		window.draw(circle);

		
	}



	//for (size_t i = 0; i < nodes.size(); ++i) {
	//	circle.setPosition(pos + sf::Vector2f{ pos.x + nodes.at(i).layer * s * 10, (float)i * s * 2 + i * s / 2});
	//	window.draw(circle);
	//}

	//window.draw(line);
}