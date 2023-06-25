#ifndef QUADTREE_H
#define QUADTREE_H

// quad tree strategy:
// divide space into quadrants, with a capacity
// once the capacity of a quadrant is exceeded,
// divide it into 4 spaces recursively

#include "Particle.h"
#include <vector>

typedef pVec2 QuadTreePoint;

struct QuadTreeNode {
    QuadTreePoint position; // position of center node
    std::vector<const Particle *> points;
    QuadTreeNode* children[4]; // Children quadrants (NW, NE, SW, SE)

	double width, height;

    QuadTreeNode(const QuadTreePoint& _position, double _width, double _height)
	: position(_position), width(_width), height(_height)
	{
        for (int i = 0; i < 4; i++) {
            children[i] = nullptr;
        }
    }
};

class QuadTree {
	private:
		QuadTreeNode *root;
		unsigned int maxPointsPerNode;

		void Clear(QuadTreeNode *node);

		void insertPoint(QuadTreeNode* node, const Particle * particle);

		void subdivideNode(QuadTreeNode* node);

		bool pointInBounds(const QuadTreeNode* node, const Particle * particle);
	
	public:
		QuadTree(const QuadTreePoint& center, double width, double height, int maxPoints)
		: maxPointsPerNode(maxPoints)
		{
        	root = new QuadTreeNode(center, width / 2.0, height / 2.0);
    	}

		~QuadTree() {
        	Clear(root);
    	}

		void insert(const Particle * particle) {
     	   insertPoint(root, particle);
    	}
};

#endif
