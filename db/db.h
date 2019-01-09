#ifndef DB_H
#define DB_H
#include "node.h"
#include "dnode.h"
#include "inode.h"
#include "node.h"
#include <sstream>
#include <string>
#include <exception>
#include <iostream>

using std::ostream;
using std::exception;
using std::string;
using std::ostringstream;

class DB {
	Node* root;
	dNode* leafHead;
public:

	DB();
	
	void NodeDump();
	void LeafDump();
	Node* Insert(int i);
	bool iNodeTest(int i);
	//throws an exception if leaf nodes are not in correct order(ascending).
	void LeafCheck();
	//traverses the entire tree from the root to the leafs to ensure leaves are in correct order
	void DepthFirstLeafCheck();
	void Remove(int key);
};

#endif
