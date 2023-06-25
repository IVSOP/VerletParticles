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

void QuadTree::insertPoint(QuadTreeNode* node, const Particle * particle) {
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

	// If the node is a leaf and has space, insert the point
	if (node->points.size() < maxPointsPerNode) {
		node->points.push_back(particle);
		return;
	}

	// If the node is a leaf but has reached capacity, subdivide it
	if (node->children[0] == nullptr) {
		subdivideNode(node);
	}

	// Try to insert the point into the appropriate child node
	for (int i = 0; i < 4; ++i) {
		if (pointInBounds(node->children[i], particle)) {
			insertPoint(node->children[i], particle);
			return;
		}
	}
}

void QuadTree::subdivideNode(QuadTreeNode* node) {
	double subWidth = node->width / 2.0;
	double subHeight = node->height / 2.0;

	node->children[0] = new QuadTreeNode(QuadTreePoint(node->position.x - subWidth, node->position.y - subHeight), subWidth, subHeight);
	node->children[1] = new QuadTreeNode(QuadTreePoint(node->position.x + subWidth, node->position.y - subHeight), subWidth, subHeight);
	node->children[2] = new QuadTreeNode(QuadTreePoint(node->position.x - subWidth, node->position.y + subHeight), subWidth, subHeight);
	node->children[3] = new QuadTreeNode(QuadTreePoint(node->position.x + subWidth, node->position.y + subHeight), subWidth, subHeight);
}

bool QuadTree::pointInBounds(const QuadTreeNode* node, const Particle * particle) {
	const pVec2 * point = &(particle->current_pos);
    return (point->x >= node->position.x - node->width / 2.0 &&
            point->x <= node->position.x + node->width / 2.0 &&
            point->y >= node->position.y - node->height / 2.0 &&
            point->y <= node->position.y + node->height / 2.0);
}

