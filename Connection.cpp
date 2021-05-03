#include "Connection.h"

#include <iostream>

Connection::Connection(Node* from, Node* to, float w, int inno) : 
	fromNode{ from }, 
	toNode{ to }, 
	weight{ w }, 
	innovationNumber{ inno } {
	

}

void Connection::mutateWeight() {
	

	if (rand() % 10 <= 8) { // 80 percent chance to slightly change

		// change it by a random number between -0.1 and 0.1
		float r = ((float(rand()) / float(RAND_MAX)) * (0.1f - (-0.1f))) + (-0.1f);

		weight += r;
	}
	else { // 20 percent chance to completely change
		weight = ((float(rand()) / float(RAND_MAX)) * (1.f - (-1.f))) + (-1.f);
	}

	// constrain the weight in case it became > 1 or < -1
	if (weight > 1) weight = 1;
	if (weight < -1) weight = -1;
}
