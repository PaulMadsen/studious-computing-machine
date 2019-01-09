#ifndef DNODE_H
#define DNODE_H
#include "node.h"
#include "myexceptions.h"
#include <exception>
#include <sstream>

class dNode : public Node {

public:
	dNode* nextNode; //linked list
	int keysUsed;
	pair<int, int> data[MAX_DNODE_KEYS];

	dNode();
	Node* Insert(int index);
	//void LeafDump(size_t level, MyStream& out); //handled by DB
	static void NodeDump(dNode* node, size_t level);
	inline void NodeDump(size_t level) { NodeDump (this, level); }
	int GetMinMax();
	bool iNodeTest(int i);
	int DepthFirstLeafCheck(int key);

	void Remove(Node* currentNode,
		int key,
		size_t currentLevel,
		Node* leftNeighbor, Node* rightNeighbor,
		Node* leftAnchor, Node* rightAnchor,
		size_t leftAnchorLevel, size_t rightAnchorLevel);
		
	void Rebalance(Node* currentNode,
		int key,
		size_t currentLevel,
		Node* leftNeighbor, Node* rightNeighbor,
		Node* leftAnchor, Node* rightAnchor,
		size_t leftAnchorLevel, size_t rightAnchorLevel) {
		throw notImplementedException();
	}
	Node* getFirstElement() { return  nullptr; }
	~dNode();
};
#endif
