#pragma once
#include <SFML/Graphics.hpp>
#include <random>
#include <ctime>


class QuadTree;

class Boid {
public:
	sf::Vector2f position;
	sf::Vector2f vel;
	sf::Vector2f acceleration;
	sf::Color color;//sf::Color(rand() % 255, rand() % 50 + 50, rand() % 255);
	float speed;
	float turningTo;
	float size = 3;

	Boid(sf::Vector2i borders) {
		position = sf::Vector2f(rand() % borders.x, rand() % borders.y);
		vel = sf::Vector2f(rand()%5-2, rand()%5-2);
		sf::Vector2f((rand() % 100 - 50), (rand() % 100 - 50));
		acceleration = sf::Vector2f(rand()%10-5, rand()%10-5);
		sf::Color(int(position.x * 0.3) % 255, 0, int(position.y * 0.3) % 255);//sf::Color(rand() % 255, rand() % 50 + 50, rand() % 255);
		speed = size*6;
	}

	void draw(sf::RenderWindow& window, float zoom);
	void update(sf::Vector2u screenSize, QuadTree quad, sf::Time dt);

	sf::Vector2f cohesion(QuadTree quad);
	sf::Vector2f alignment(QuadTree quad);
	sf::Vector2f separation(QuadTree quad);

};