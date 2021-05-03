#include "Application.h"



void Application::initVars() {
	m_window = nullptr;

}

void Application::initWindow() {
	m_videoMode.width = 800;
	m_videoMode.height = 600;

	sf::ContextSettings settings{};
	settings.antialiasingLevel = 16;

	m_window = new sf::RenderWindow(m_videoMode, "NN", sf::Style::Titlebar | sf::Style::Close, settings);

}

Application::Application() : nn{ 4, 1 } {
	initVars();
	initWindow();

	// This code is run upon running the application, use for testing

	std::vector<ConnectionHistory> a;
	for (int i = 0; i < 2; ++i) {
		nn.mutate(a);
	}


	std::vector<float> in{ 5.f, 3.f, 0.5f, 10.f };
	std::cout << "Inputs: ";
	for (auto& i : in) std::cout << i << ", ";
	std::cout << '\n';

	in = Genome::normalize(in);
	std::cout << "Normalized inputs: ";
	for (auto& i : in) std::cout << i << ", ";
	std::cout << "\n\n";

	std::vector<float> out;
	out = nn.getOutputs(in);

	std::cout << "Outputs: ";
	for (size_t i = 0; i < out.size(); ++i)
		std::cout << out.at(i) << ", ";




}

Application::~Application() {
	delete m_window;
}

const bool Application::running() const {
	return m_window->isOpen();
}

void Application::pollEvents() {
	while (m_window->pollEvent(m_event)) {
		switch (m_event.type) {
		case sf::Event::Closed:
			m_window->close();
			break;

		case sf::Event::KeyPressed:
			switch (m_event.key.code) {
			case sf::Keyboard::Escape:
				m_window->close();
				break;

			}

			break;
		}

	}
}

void Application::update() {
	pollEvents();

	//std::cout << std::endl;
	//Genome n{ 2, 1 };

	//std::vector<ConnectionHistory> a;
	//for (int i = 0; i < 10; ++i) {
	//	n.mutate(a);
	//}


	//std::vector<float> in{ -0.6f, 0.2f };
	//in = Genome::normalize(in);

	//std::vector<float> out;
	//out = n.getOutputs(in);

	//for (size_t i = 0; i < out.size(); ++i) {
	//	std::cout << out.at(i) << " ";
	//}
	

}

void Application::render() {
	// Clear
	m_window->clear(sf::Color::White);


	// Render

	nn.draw(*m_window, sf::Vector2f(40, 200), sf::Vector2f(400, 400), 4);


	m_tile.setFillColor(sf::Color::Green);
	m_tile.setSize(sf::Vector2f(10.f, 10.f));
	m_tile.setPosition(40, 200);
	m_window->draw(m_tile);
	m_tile.setPosition(400, 400);
	m_window->draw(m_tile);


	// Display
	m_window->display();

}