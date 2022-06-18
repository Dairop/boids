#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include "boid.h"


struct rectByCenter {
	sf::Vector2f center;
	sf::Vector2f radius;
};


class QuadTree
{
public:
	// Constante arbitraire indiquant combien d'éléments peuvent être stockés dans ce nœud de quadtree
	const int QT_NODE_CAPACITY = 4;

	// Zone de délimitation alignée sur l'axe (représentée par sa demi-dimension et son centre)
	// représentant les limites de ce quadtree
	rectByCenter boundary;

	// Points de ce nœeud de quadtree
	std::vector <Boid*> points;


	// Méthodes
	QuadTree(rectByCenter bd) { boundary = bd; };
	bool insert(Boid* p);
	void subdivide(); // créer quatre enfants permettant de diviser ce quadrant en quatre quadrants d'égales dimensions
	void queryRangeRect(rectByCenter range, std::vector<Boid> & pointsInRange);
	void queryRangeCircle(rectByCenter range, std::vector<Boid>& pointsInRange);
	void display(sf::RenderWindow&);
	void del();  // delete the QuadTree from the actual pos. to the leafs

	// Enfants/*

	QuadTree* northWest;
	QuadTree* northEast;
	QuadTree* southWest;
	QuadTree* southEast;
};

