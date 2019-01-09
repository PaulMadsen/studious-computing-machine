#include "db.h"

DB::DB() {
	root = leafHead = new dNode(); //asuming fresh db

}
void DB::NodeDump()
{
	cout << "Dump of B+ Tree\n";
	root->NodeDump(0);
}
void dumpOneLeaf(dNode* leaf, ostringstream& o) {
	for (int i = 0; i < leaf->keysUsed; ++i) 
		o << leaf->data[i].first << " ";
	o << endl;
}
void dumpOneNode(iNode* node, ostringstream& o) {
	o << "parent node: ";
	if (node == nullptr){
		o << "NULL";
		return;
	}
	o << "* ";
	for (int i = 0; i < node->keysUsed; ++i)
		o << node->keys[i] << " * ";
	o << "*\n";
}
void DB::LeafCheck()
{
	dNode* current;
	dNode* previous = current = static_cast<dNode*>(leafHead);
	int currentVal = current->data->first;
	int i = 1;
	do {
		for (; i < current->keysUsed; ++i) {
			if (current->data[i].first <= currentVal){
				ostringstream oStream;
				oStream << "ERROR: Leaf nodes are not in correct order\n";
				if (previous != current)
					dumpOneLeaf(previous, oStream);
				dumpOneLeaf(current, oStream);
				throw std::logic_error(oStream.str());
			}
			currentVal = current->data[i].first;
		}
		i = 0;
		previous = current;
		current = static_cast<dNode*>(current->nextNode);
	} while (current != nullptr);
	
}
void DB::LeafDump() {
	cout << "Leaves of B+Tree\n";
	dNode* current;
	dNode* next = current = static_cast<dNode*>(leafHead);
	do {
		cout << "    [ ";
		for (int i = 0; i < current->keysUsed; ++i) {
			cout << current->data[i].first;
			if (i + 1 != current->keysUsed)
				cout << ", ";
		}
		cout << "]";
		if (current->nextNode == nullptr)
			cout << ".";
		else
			cout << ",";
		cout << "\n";
		current = static_cast<dNode*>(current->nextNode);
	} while (current != nullptr);
}
Node* DB::Insert(int key) {
		
	Node* newMaxNode = root->Insert(key);
	if (newMaxNode != nullptr) { //a split has occured
		iNode* newRoot = new iNode();
		newRoot->keys[0] = newMaxNode->GetMinMax();
		newRoot->children[0] = root;
		newRoot->children[1] = newMaxNode;
		newRoot->keysUsed++;
		root = newRoot;
	}
	return nullptr;
}
bool DB::iNodeTest(int i) {
	return root->iNodeTest(i);
}
void DB::DepthFirstLeafCheck() {
	root->DepthFirstLeafCheck(leafHead->data[0].first - 1); //starting value is one less than the left most key
}
void DB::Remove(int key) {
	root->Remove(root, key, 0, nullptr, nullptr, nullptr, nullptr, 0, 0);
	if (root->keysUsed == 0) {
		Node* tmp = root->getFirstElement();
		if (tmp)
			root = tmp;
	}
}