#ifndef INODE_H
#define INODE_H
#include "node.h"
const size_t MAX_INODE_KEYS = 4;
const size_t MIN_INODE_KEYS = MAX_INODE_KEYS / 2;
class iNode : public Node {
	int* minMax;
public:
	
	int keys[MAX_INODE_KEYS];
	Node* children[MAX_INODE_KEYS + 1];

	Node* Insert(int key);
	inline void NodeDump(size_t level) { NodeDump(this, level); }
	static void NodeDump(iNode* node, size_t level);
	iNode();
	int GetMinMax();
	bool iNodeTest(int i);
	int DepthFirstLeafCheck(int key);
	static int findAnchorPivot(iNode* anchor, int key);
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
		size_t leftAnchorLevel, size_t rightAnchorLevel);
	Node* getFirstElement() { return children[0]; }

	~iNode();
};
#endif