#pragma once





class Population {
public:

	Population();

	// updates all of the species
	void update();

	// returs whether all agents are finished
	bool doneUpdating();

	// Do this after population is done updating
	// does genetic algorithm:
	// 1. get fitness of all agents
	// 2. speciate
	// 3. select one agent from each species
	// 4. clone and mutate those agents to make a new population
	void createNewPopulation();


private:

	// for each species, kill half of the population
	void cull();

};

