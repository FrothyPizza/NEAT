#pragma once


#include <iostream>
#include <vector>

#include "Genome.h"

class Genome;

/* 
	Every time there is a new innovation, a new ConnectionHistory must be made
	The new connection history stores:
	the node the connection is from and to as ints,
	the innovation number, and 
	a vector of ints storing innovations
*/
class ConnectionHistory {
public:

	// list of all innovations for this connection histry
	std::vector<int> innovations;

	// the node the newest innovation (connection) is from
	int fromNodeNo;

	// the node the newest innovation (connection) is to
	int toNodeNo;

	// the cooresponding innovation number
	int innovationNo;


	ConnectionHistory(std::vector<int> innoNos, int from, int to, int innoNo);

	// retruns whether the genome passed in matches and if the new connection matches this from and to node
	bool sameAs(const Genome& genome, const Node& from, const Node& to);

};

