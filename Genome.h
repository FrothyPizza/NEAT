#pragma once

#include <vector>
#include <iostream>

#include <vector>

#include <SFML/Graphics.hpp>
#include "Line.h"

#include "Node.h"
#include "ConnectionHistory.h"

class ConnectionHistory;

/*
	This is a Neural Network
	There is no crossover because i'm too lazy for that and it shouldn't hurt performance too much
	Instead of crossover, the best genome of each species in the population will just clone itself a bunch and mutate the clones
*/
class Genome {
public:

	std::vector<Connection> connections;
	std::vector<Node> nodes;

	// the nodes aren't ever in order, so the network has to be ordered and this is used for the feeding forward
	std::vector<Node> orderedNetwork;

	int inputs;
	int outputs;

	int biasNode; // id of the bias node
	int nextNode; // the id of the next node to be added
	int nextConnection; // the id of the next connection
	int layers{ 2 }; // the number of layers in the NN, starts at 2 (inputs/outputs)

	// order the NN for use
	void orderNetwork();
	
	Genome(int inputs, int outputs);
	Genome(Genome& other);

	// no crossover
	//// returns a new genome that is a cross between the caller and the one passed in
	//// for now the baby inherits all genes from the parents
	//// I couldn't figure out how to do this, so i guess it's not going to be implemented for now
	//Genome reproduce(Genome& other);


	// mutates genes and has a small chance of adding a node and/or connection
	// pass in the innovation history so that it can be altered if nodes or connections are added
	void mutate(std::vector<ConnectionHistory>& innovationHistory);

	// feeds forward and returns a vector of the outputs
	std::vector<float> getOutputs(std::vector<float> inputs);

	// pass in: the window to be drawn to, bounding box based on top left and bottom right, and the scale
	void draw(sf::RenderWindow& window, const sf::Vector2f& startpos, const sf::Vector2f& endpos, float scale);

private:

	// mutates by adding a node
	// disables a connection and adds two in place of it
	// the new connection going to the new node has a weight of 1
	// the new connection going out of the node has the same weight as the disabled connection
	void addNode(std::vector<ConnectionHistory>& innovationHistory);

	// adds a connection between two unconnected nodes
	// doesn't do anything if the nn is fully connected
	void addConnection(std::vector<ConnectionHistory>& innovationHistory);

	// gets the innovation number based on the in and out node
	// if the innovation hasn't been made before, add a new ConnectionHistory to innovationHistory representing that innovation
	int getInnovationNumber(std::vector<ConnectionHistory>& innovationHistory, Node& from, Node& to); 

	// sets the connections of each node to the corresponding weight
	void connectNodes();





public:
	// normalization code for the inputs
	static std::vector<float> normalize(const std::vector<float>& arr) {
		std::vector<float> output{};
		float mod = 0.0;

		for (size_t i = 0; i < arr.size(); ++i) {
			mod += arr[i] * arr[i];
		}

		float mag = std::sqrt(mod);

		if (mag == 0) {
			throw std::logic_error("The input vector is a zero vector");
		}

		for (size_t i = 0; i < arr.size(); ++i) {
			output.push_back(arr[i] / mag);
		}

		return output;
	}
};

