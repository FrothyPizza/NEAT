#include "ConnectionHistory.h"


#include <algorithm>


ConnectionHistory::ConnectionHistory(std::vector<int> innoNos, int from, int to, int innoNo) : 
innovations{ innoNos }, fromNodeNo{ from }, toNodeNo{ to }, innovationNo{ innoNo } {
	

}

bool ConnectionHistory::sameAs(const Genome& genome, const Node& from, const Node& to) {

	// if the sizes of the innovations aren't the same, return false
	if (genome.connections.size() != innovations.size()) return false;

	// if the node numbers are the same
	if (from.number == fromNodeNo && to.number == toNodeNo) {
		for (size_t i = 0; i < genome.connections.size(); ++i) {
			// if this innovations doesn't have the innovation number of the genome's connection at i, then return false
			if (!(std::find(innovations.begin(), innovations.end(), genome.connections.at(i).innovationNumber) != innovations.end()))
				return false;
		}

		// if it's all the same, return true
		return true;
	}


	return false;
}
