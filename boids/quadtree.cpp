#include "quadtree.h"

float distSqrd(sf::Vector2f p1, sf::Vector2f p2) {
    return (p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y);  // return squared distance
}

bool collideRectAndRect(rectByCenter rect1, rectByCenter rect2) {
    float tlx1 = rect1.center.x - rect1.radius.x;
    float tly1 = rect1.center.y - rect1.radius.y;
    float brx1 = rect1.center.x + rect1.radius.x;
    float bry1 = rect1.center.y + rect1.radius.y;

    float tlx2 = rect2.center.x - rect2.radius.x;
    float tly2 = rect2.center.y - rect2.radius.y;
    float brx2 = rect2.center.x + rect2.radius.x;
    float bry2 = rect2.center.y + rect2.radius.y;

    if ((tlx1 <= brx2) && (brx1 >= tlx2) && (tly1 <= bry2) && (bry1 >= tly2)) {
        return true;
    }
    return false;
}

void QuadTree::del() {
    if (northWest != nullptr) {
        northWest->del();
        northEast->del();
        southEast->del();
        southWest->del();
    }
    delete northWest;
    delete northEast;
    delete southEast;
    delete southWest;

    points.clear();
    return;
}

void QuadTree::display(sf::RenderWindow &window) {
    sf::RectangleShape rect;
    rect.setOutlineColor(sf::Color(150, 150, 150));
    rect.setOutlineThickness(1);
    rect.setFillColor(sf::Color(0, 255, 0, 0));
    sf::Vector2f pos;

    pos = sf::Vector2f(boundary.center.x - boundary.radius.x, boundary.center.y - boundary.radius.y);
    rect.setPosition(pos);
    rect.setSize(sf::Vector2f(boundary.radius.x * 2, boundary.radius.y * 2));
    window.draw(rect);
    if (northWest == nullptr) {
        return;
    }
    else {
        northWest->display(window);
        northEast->display(window);
        southEast->display(window);
        southWest->display(window);
        return;
    }
}

void QuadTree::subdivide() {
    //subdivide the previous rect in 4 equal parts
    rectByCenter r;
    r.radius = sf::Vector2f(boundary.radius.x /2, boundary.radius.y /2);
    r.center.x = boundary.center.x - boundary.radius.x / 2;
    r.center.y = boundary.center.y - boundary.radius.y / 2;
    //QuadTree nw = new QuadTree (r);
    northWest = new QuadTree(r);

    r.center.x = boundary.center.x + boundary.radius.x / 2;
    r.center.y = boundary.center.y - boundary.radius.y / 2;
    //QuadTree ne(r);
    northEast = new QuadTree(r);

    r.center.x = boundary.center.x + boundary.radius.x / 2;
    r.center.y = boundary.center.y + boundary.radius.y / 2;
    //QuadTree se(r);
    southEast = new QuadTree(r);

    r.center.x = boundary.center.x - boundary.radius.x / 2;
    r.center.y = boundary.center.y + boundary.radius.y / 2;
    //QuadTree sw(r);
    southWest = new QuadTree(r);
};

bool QuadTree::insert(Boid *p){
    // Insérer un point dans le QuadTree
    // Ignorer les objets qui n'appartiennent pas a ce quadtree
    if ((abs(boundary.center.x - p->position.x) >= boundary.radius.x) || (abs(boundary.center.y - p->position.y) >= boundary.radius.y)) {
        return false; // l'objet ne doit pas être ajoute
    }

    // S'il reste de la place dans ce quadtree, y ajouter l'objet
    if (points.size() < QT_NODE_CAPACITY && northWest == nullptr) {
        points.push_back(p);
        return true;
    }

    // Sinon, subdiviser le quadtree, puis ajouter le point au nœud qui l'acceptera
    if (northWest == nullptr) {
        subdivide();

        for (int po = 0; po < points.size(); po++) {
            insert(points[po]);
        }
        points.clear();
    }


    if (northWest->insert(p)) return true;
    if (northEast->insert(p)) return true;
    if (southWest->insert(p)) return true;
    if (southEast->insert(p)) return true;

    // Sinon, le point ne peut etre insere, pour une raison inconnue (cela ne devrait jamais arriver)
    return false;
}

void QuadTree::queryRangeRect(rectByCenter range, std::vector<Boid> &pointsInRange) {
    // skip if the quadtree isn't concerned
    if (!collideRectAndRect(range, boundary)) {
        return;
    }

    //if not child, return the node points
    if (northWest == nullptr) {
        for (int p = 0; p < points.size(); p++) {
            if ((abs(points.at(p)->position.x - range.center.x) < range.radius.x) && 
                (abs(points.at(p)->position.y - range.center.y) < range.radius.y)) {
                pointsInRange.push_back(* points.at(p));
            }
        }
        return;
    }

    // if it have childs, ask them
    northWest->queryRangeRect(range, pointsInRange);
    northEast->queryRangeRect(range, pointsInRange);
    southEast->queryRangeRect(range, pointsInRange);
    southWest->queryRangeRect(range, pointsInRange);
    return;
}

void QuadTree::queryRangeCircle(rectByCenter range, std::vector<Boid>& pointsInRange) {
    // skip if the quadtree isn't concerned
    if (!collideRectAndRect(range, boundary)) {
        return;
    }

    //if not child, return the node points
    if (northWest == nullptr) {
        for (int p = 0; p < points.size(); p++) {
            if ((abs(points.at(p)->position.x - range.center.x) <= range.radius.x) &&
                (abs(points.at(p)->position.y - range.center.y) <= range.radius.y)) {

                //in rect, check if in circle
                if (distSqrd(points[p]->position, range.center) <= range.radius.x * range.radius.x + range.radius.y * range.radius.y) {
                    pointsInRange.push_back(*points.at(p));
                }
            }
        }
        return;
    }


    // if it have childs, ask them
    northWest->queryRangeCircle(range, pointsInRange);
    northEast->queryRangeCircle(range, pointsInRange);
    southEast->queryRangeCircle(range, pointsInRange);
    southWest->queryRangeCircle(range, pointsInRange);
    return;
}
