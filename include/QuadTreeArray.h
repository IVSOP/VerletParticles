#ifndef QUADTREEARRAY_H
#define QUADTREEARRAY_H

// quad tree strategy:
// divide space into quadrants, with a capacity
// once the capacity of a quadrant is exceeded,
// divide it into 4 spaces recursively

#include "Particle.h"
#include <vector>

typedef pVec2 QuadTreePoint;




#include <iostream>




struct QuadTreeNode {
    QuadTreePoint position; // position of center node
    std::vector<Particle *> points;
	double half_width, half_height;

    QuadTreeNode(const QuadTreePoint& _position, double _half_width, double _half_height)
	: position(_position), half_width(_half_width), half_height(_half_height)
	{
    }
};

class QuadTreeArray {
	private:
		unsigned int maxPointsPerNode;
		unsigned int capacity;
		QuadTreeNode **nodes;

		void Clear(QuadTreeNode *node);
		void insertPoint(QuadTreeNode* node, Particle * particle);
		void subdivideNode(QuadTreeNode* node);
		bool pointInBounds(const QuadTreeNode* node, const Particle * particle);
	
	public:
		QuadTreeNode *root; // easier to loop through it this way

		QuadTreeArray(const QuadTreePoint& center, double width, double height, unsigned int _maxPointsPerNode, unsigned int _capacity)
		: maxPointsPerNode(_maxPointsPerNode), capacity(_capacity)
		{
			nodes = new QuadTreeNode*[capacity];
        	root = new QuadTreeNode(center, width / 2.0, height / 2.0);

			unsigned int i;
			for (i = 0; i < capacity; i++) {
				nodes[i] = nullptr;
			}

			// in this tree, there is no root. it is assumed there are already 4 quadrants, otherwise I have nowhere to put the first node *
    	}

		void clearTree();

		~QuadTreeArray() {
			dumpTree();
        	Clear(root);
    	}

		void insert(Particle * particle) {
     	   insertPoint(root, particle);
    	}

		void solveCollisions(void ( *collideParticles) (Particle *p1, Particle *p2));
		void solveCollisionsInNode(QuadTreeNode *node, void ( *collideParticles) (Particle *p1, Particle *p2));
		void checkCollisionsInNeighboringQuadrants(QuadTreeNode *node, void ( *collideParticles) (Particle *p1, Particle *p2));

		void dumpTree();
};

#endif
