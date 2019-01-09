#ifndef NODE_H
#define NODE_H
#include <cstddef>
#include <string>
#include <cassert>
#include <iostream>
#include <utility>
#include <sstream>
#include "myexceptions.h"
#include <sstream>
using std::ostringstream;
using std::ostream;
using std::string;
using std::pair;
using std::cout;
using std::size_t;
using std::endl;


const size_t MAX_DNODE_KEYS = 4;
const size_t MIN_DNODE_KEYS = MAX_DNODE_KEYS / 2;

class Node {
public:
	int keysUsed;
	virtual Node* Insert(int index) = 0;
	virtual int GetMinMax() = 0;
	Node();
	virtual void NodeDump(size_t level) = 0;
	virtual bool iNodeTest(int i) = 0;
	virtual int DepthFirstLeafCheck(int key) = 0;
	virtual void Remove(Node* currentNode,
		int key,
		size_t currentLevel,
		Node* leftNeighbor, Node* rightNeighbor,
		Node* leftAnchor, Node* rightAnchor,
		size_t leftAnchorLevel, size_t rightAnchorLevel) = 0;
	virtual void Rebalance(Node* currentNode,
		int key,
		size_t currentLevel,
		Node* leftNeighbor, Node* rightNeighbor,
		Node* leftAnchor, Node* rightAnchor,
		size_t leftAnchorLevel, size_t rightAnchorLevel) = 0;
	virtual Node* getFirstElement() = 0;
	~Node(){ }
};

#endif