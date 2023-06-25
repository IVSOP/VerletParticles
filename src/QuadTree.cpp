#include "QuadTree.h"

void QuadTree::Clear(QuadTreeNode *node) {
	// changed this initial check, moved it into the for loop
	// if (node == nullptr) {
    // 	return;
	// }

	int i;

	for (i = 0; i < 4 && node->children[i] != nullptr; i++) {
		Clear(node->children[i]);
	}

	delete node;
}

void QuadTree::insertPoint(QuadTreeNode* node, Particle * particle) {
	// changed this initial check, moved it into the for loop
	// if (node == nullptr) {
	// 	return;
	// }

	int i;
	// If the node has children, try to insert the point into them
	for (i = 0; i < 4 && node->children[i] != nullptr; ++i) {
		if (pointInBounds(node->children[i], particle)) {
			insertPoint(node->children[i], particle);
			return;
		}
	}

	node->points.push_back(particle);

    // Check if the node needs to be subdivided
    if (node->points.size() > maxPointsPerNode) {
        // if (node->children[0] == nullptr) { redundant check, if it had children it would not have filled up
            // Subdivide the node if it's a leaf node
            subdivideNode(node);
        // }

		// this could also be done in subdivision, idk
        // Redistribute existing points among child nodes
        for (Particle * existingParticle : node->points) {
			// Place it in the first child where it fits
            for (i = 0; i < 4; i++) {
                if (pointInBounds(node->children[i], existingParticle)) { // bug: point is never within bounds
                    insertPoint(node->children[i], existingParticle);
                    break;
                }
				else {
					// std::cout << "Point was not in bounds. particle position: " << particle->current_pos.x << "," << particle->current_pos.y << std::endl;
					// std::cout << "node position: " << node->children[i]->position.x << "," << node->children[i]->position.y << " half_width: " << node->children[i]->half_width << " half_height: " << node->children[i]->half_height << std::endl;
				}
            }
        }

        node->points.clear();
    }

	// points that were already there now go into sublevels
}

void QuadTree::subdivideNode(QuadTreeNode* node) {
	double subWidth = node->half_width / 2.0;
	double subHeight = node->half_height / 2.0;

	node->children[0] = new QuadTreeNode(QuadTreePoint(node->position.x - subWidth, node->position.y - subHeight), subWidth, subHeight);
	node->children[1] = new QuadTreeNode(QuadTreePoint(node->position.x + subWidth, node->position.y - subHeight), subWidth, subHeight);
	node->children[2] = new QuadTreeNode(QuadTreePoint(node->position.x - subWidth, node->position.y + subHeight), subWidth, subHeight);
	node->children[3] = new QuadTreeNode(QuadTreePoint(node->position.x + subWidth, node->position.y + subHeight), subWidth, subHeight);
}

// is this faster than just checking the distance to the center point??
bool QuadTree::pointInBounds(const QuadTreeNode* node, const Particle * particle) {
	const pVec2 * point = &(particle->current_pos);

	// std::cout << (point->x >= node->position.x - (node->width / 2.0)) << (point->x <= node->position.x + (node->width / 2.0)) << (point->y >= node->position.y - (node->height / 2.0)) << (point->y <= node->position.y + (node->height / 2.0)) << std::endl;

	// std::cout << point->x << std::endl;
	// std::cout << node->position.x << std::endl;
	// std::cout << node->width << std::endl;
	// std::cout << node->position.x - (node->width / 2.0) << std::endl;

    return (point->x >= node->position.x - node->half_width &&
            point->x <= node->position.x + node->half_width &&
            point->y >= node->position.y - node->half_height &&
            point->y <= node->position.y + node->half_height);
}

void QuadTree::solveCollisionsInNode(QuadTreeNode *node, void ( *collideParticles) (Particle *p1, Particle *p2)) {
	unsigned int i;
	// go as deep as possible
	for (i = 0; i < 4 && node->children[i] != nullptr; i++) {
		solveCollisionsInNode(node->children[i], collideParticles);
	}

	// no children, have to solve collisions here
	// is this check needed, I could just look at the vector it has
	if (i == 0) {
		Particle *p1, *p2;
		std::vector<Particle *> &vec = node->points;
		unsigned int j, size = vec.size();

		for (i = 0; i < size; i++) {
			p1 = vec[i];
			for (j = i + 1; j < size; j++) { // is this ok?????????????????????????
				p2 = vec[j];
				collideParticles(p1, p2);
			}
		}
	}
}

void QuadTree::solveCollisions(void ( *collideParticles) (Particle *p1, Particle *p2)) {
	solveCollisionsInNode(root, collideParticles);
}

#include <iostream>
#include <stdio.h>

void printSpace (int depth) {
    for (; depth; depth--) printf("  ");
}

void _dumpTree(QuadTreeNode *node, int depth) {
	printSpace(depth);
	puts("[");
	printSpace(depth);
	printf("%ld particles here\n", node->points.size());

	unsigned int i;
	for (i = 0; i < 4 && node->children[i] != nullptr; i++) {
		_dumpTree(node->children[i], depth + 4);
	}

	printSpace(depth);
	puts("]");
}

void QuadTree::dumpTree() {
	_dumpTree(root, 0);
}
