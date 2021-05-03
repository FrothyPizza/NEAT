#pragma once
#include <SFML/Graphics.hpp>

#include <iostream>

#include "Genome.h"

class Application {

private:

	sf::RenderWindow* m_window;
	sf::Event m_event;
	sf::VideoMode m_videoMode;

	Genome nn;

	float m_tileSize;
	sf::RectangleShape m_tile;


	void initVars();
	void initWindow();


public:

	Application();
	virtual ~Application();

	const bool running() const;

	void pollEvents();
	void update();
	void render();

};