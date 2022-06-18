#pragma once

#ifndef iosteam
#include <iostream>
#include <string.h>
#include <cmath>
#include <vector>
#include <SFML/Graphics.hpp>

#include "boid.h"
#include "quadtree.h"
#endif
//#include "quadtree.h"



float dist(sf::Vector2f p1, sf::Vector2f p2) {
	return sqrt((p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y));
}


sf::Vector2f normalize(sf::Vector2f vect) {
	float d = dist(sf::Vector2f(0, 0), vect) + 0.0000001;
	return sf::Vector2f(vect.x / d, vect.y / d);
}

void Boid::draw(sf::RenderWindow& window, float zoom) {
	sf::CircleShape c;
	c.setFillColor(color);//sf::Color(int (position.x) %255, int(position.y) %255, int(position.x + 125) % 255));
	c.setPosition(sf::Vector2f(position.x-size, position.y-size));
	c.setRadius(this->size);
	window.draw(c);
}

sf::Vector2f Boid::alignment(QuadTree quad) {
	std::vector<Boid> nearPoints;
	rectByCenter range;
	range.center = position;
	range.radius = sf::Vector2f(this->size * 10, this->size * 10);
	quad.queryRangeCircle(range, nearPoints);

	sf::Vector2f steering = sf::Vector2f(0, 0);

	if (nearPoints.size() > 0) {
		for (int p = 0; p < nearPoints.size(); p++) {
			steering.x += nearPoints[p].vel.x;
			steering.y += nearPoints[p].vel.y;
		}
		steering.x /= nearPoints.size();
		steering.y /= nearPoints.size();

		//normalize
		steering = normalize(steering);

		steering.x *= speed;
		steering.y *= speed;

		steering.x -= vel.x;
		steering.y -= vel.y;

	}
	return steering;

}


sf::Vector2f Boid::cohesion(QuadTree quad) {
	std::vector<Boid> nearPoints;
	rectByCenter range;
	range.center = position;
	range.radius = sf::Vector2f(this->size * 8.5, this->size * 8.5);
	quad.queryRangeCircle(range, nearPoints);

	sf::Vector2f avgPoint;
	sf::Vector2f steering = sf::Vector2f(0, 0);
	avgPoint.x = 0;
	avgPoint.y = 0;
	if (nearPoints.size() > 0) {
		for (int p = 0; p < nearPoints.size(); p++) {
			avgPoint.x += nearPoints[p].position.x;
			avgPoint.y += nearPoints[p].position.y;
		}
		avgPoint.x /= nearPoints.size();
		avgPoint.y /= nearPoints.size();

		//normalize
		steering = avgPoint;

		steering.x -= position.x;
		steering.y -= position.y;

		steering = normalize(steering);

		steering.x *= speed;
		steering.y *= speed;

		steering.x -= vel.x;
		steering.y -= vel.y;

	}
	return steering;

}

sf::Vector2f Boid::separation(QuadTree quad) {
	std::vector<Boid> nearPoints;
	rectByCenter range;
	range.center = position;
	range.radius = sf::Vector2f(this->size * 4, this->size * 4);
	quad.queryRangeCircle(range, nearPoints);

	sf::Vector2f diff;
	sf::Vector2f steering = sf::Vector2f(0, 0);

	if (nearPoints.size() > 0) {
		for (int p = 0; p < nearPoints.size(); p++) {
			diff.x = position.x - nearPoints[p].position.x;
			diff.y = position.y - nearPoints[p].position.y;

			//Squared dist
			float d = (nearPoints[p].position.x - position.x) * (nearPoints[p].position.x - position.x) + 
					  (nearPoints[p].position.y - position.y) * (nearPoints[p].position.y - position.y);
			if (d < 1){
				diff.x *= 1/(d + 0.0001);
				diff.y *= 1/(d + 0.0001);
			}
			
			steering.x += diff.x;
			steering.y += diff.y;
		}
		steering.x /= nearPoints.size();
		steering.y /= nearPoints.size();

		steering = normalize(steering);
		steering.x *= speed;
		steering.y *= speed;

		steering.x -= vel.x;
		steering.y -= vel.y;
	}
	return steering;

}


void Boid::update(sf::Vector2u screenSize, QuadTree quad, sf::Time dt) {
	//init
	acceleration.x = 0;
	acceleration.y = 0;

	//bounce on walls
	if (position.x < 0) {
		acceleration.x = 20;
	}
	else 	if (position.x > screenSize.x) {
		acceleration.x = -20;

	}
	if (position.y < 0) {
		acceleration.y = 20;

	}
	else if (position.y > screenSize.y) {
		acceleration.y = -20;

	}
	//acceleration = normalize(acceleration);

	sf::Vector2f a;
	a = alignment(quad);
	acceleration.x += a.x * 0.5;
	acceleration.y += a.y * 0.5;
	a = cohesion(quad);
	acceleration.x += a.x * 0.05;
	acceleration.y += a.y * 0.05;
	a = separation(quad);
	acceleration.x += a.x * 3;
	acceleration.y += a.y * 3;


	vel.x += acceleration.x/50;
	vel.y += acceleration.y/50;
	vel = normalize(vel);

	position.x += vel.x * speed * dt.asMilliseconds()/1000;
	position.y += vel.y * speed * dt.asMilliseconds()/1000;


}
