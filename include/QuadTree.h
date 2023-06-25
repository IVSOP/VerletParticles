#ifndef QUADTREE_H
#define QUADTREE_H

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
    QuadTreeNode* children[4]; // Children quadrants (NW, NE, SW, SE)

	double half_width, half_height;

    QuadTreeNode(const QuadTreePoint& _position, double _half_width, double _half_height)
	: position(_position), half_width(_half_width), half_height(_half_height)
	{
        for (int i = 0; i < 4; i++) {
            children[i] = nullptr;
        }
		// std::cout << "position: " << position.x << "," << position.y << " width: " << width << " height: " << height << std::endl;
    }
};

class QuadTree {
	private:
		unsigned int maxPointsPerNode;

		void Clear(QuadTreeNode *node);

		void insertPoint(QuadTreeNode* node, Particle * particle);

		void subdivideNode(QuadTreeNode* node);

		bool pointInBounds(const QuadTreeNode* node, const Particle * particle);
	
	public:
		QuadTreeNode *root; // easier to loop through it this way

		QuadTree(const QuadTreePoint& center, double width, double height, int maxPoints)
		: maxPointsPerNode(maxPoints)
		{
        	root = new QuadTreeNode(center, width / 2.0, height / 2.0);
    	}

		~QuadTree() {
			dumpTree();
        	Clear(root);
    	}

		void insert(Particle * particle) {
     	   insertPoint(root, particle);
    	}

		void solveCollisions(void ( *collideParticles) (Particle *p1, Particle *p2));
		void solveCollisionsInNode(QuadTreeNode *node, void ( *collideParticles) (Particle *p1, Particle *p2));

		void dumpTree();
};

#endif
