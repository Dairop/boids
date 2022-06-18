#include <SFML/Graphics.hpp>
#include <iostream>
#include <random>
#include <chrono>

#include "quadtree.h"

#include "boid.h"


int main() {
	bool pause = true;

	std::srand(std::time(nullptr));

	//load color palette
	sf::Image tp;
	int windowWidth = 1920;
	int windowHeight = 1080;

	std::vector<Boid> *boidsV = new std::vector<Boid>;
	for (int i = 0; i < 5000; i++) {
		Boid mt(sf::Vector2i(windowWidth, windowHeight)); //new one each time, so they get a new orientation & pos
		boidsV->push_back(mt);
		int g = 1+rand() % 200;
		boidsV->at(i).color = sf::Color(rand()%70+185, g, rand()%g);
		/*
		try {
			boidsV->at(i).color = tp.getPixel(boidsV->at(i).position.x, boidsV->at(i).position.y);
		} catch(...) {
			boidsV->at(i).color = sf::Color::Blue;
		}*/
	}

	sf::Clock deltaClock;
	sf::Time dt = deltaClock.restart();

	sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Name");
	const sf::Vector2i viewCenter = sf::Vector2i(-9, -38);
	window.setPosition(viewCenter);
	window.clear(sf::Color::Black);

	//fill screen with a transparent black layer
	sf::RectangleShape black;
	black.setSize(sf::Vector2f(windowWidth, windowHeight));
	black.setFillColor(sf::Color(0, 0, 0, 30));
	black.setPosition(sf::Vector2f(0, 0));


	while (window.isOpen()) {
		sf::Event event;
		rectByCenter screenRect;
		screenRect.center = sf::Vector2f(windowWidth/2, windowHeight/2); screenRect.radius = sf::Vector2f(windowWidth/2, windowHeight/2);
		QuadTree *bigQuad = new QuadTree(screenRect);

		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window.close();
			}
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) {
			pause = false;// !pause;
		}

		//window.clear();
		window.draw(black);

		for (int b = 0; b < boidsV->size(); b++) {
			bigQuad->insert(&boidsV->at(b));
		}

		//bigQuad->display(window);
		for (int b = 0; b < boidsV->size(); b++) {
			if (!pause)
				boidsV->at(b).update(window.getSize(), *bigQuad, dt);
			boidsV->at(b).draw(window, 1);
		}
		
		dt = deltaClock.restart();
		window.display();
		bigQuad->del();
		delete bigQuad;
	}
	return 0;
};