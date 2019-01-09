#include "dnode.h"
#include "inode.h"
dNode::dNode() : Node() {
	keysUsed = 0;
	nextNode = nullptr;
}

int dNode::GetMinMax() {
	return data[0].first;
}
ostream& PrintDataNode(dNode* d, ostream& o) {
	o << "( ";
	for (int i = 0; i < d->keysUsed; ++i)
		o << "[" << d->data[i].first << "," << d->data[i].second << "], ";
	o << "]" << endl;
	return o;
}
Node* dNode::Insert(int key) {
	dNode* maxNode = nullptr;
	bool insertSuccess = false;
	//debuging
	//if (key == 19912)//break point
	//	cout << "";
	for (int i = 0; i < keysUsed; ++i) {
		if (data[i].first == key) { return maxNode; }//key already present.  No op
		if (key < data[i].first) {
			if (keysUsed == MAX_DNODE_KEYS) {//leaf node needs to split.  Insertion will be handled by case below
				break;
			}
			int j = 0;
			for (j = keysUsed; j > 0 && data[j - 1].first > key; --j) {//shift right
				data[j].first = data[j - 1].first;
				data[j].second = data[j - 1].second;
			}
			data[j].first = data[j].second = key;
			++keysUsed;
			return maxNode;
		}
	}
	if (keysUsed == 0) { //root case
		data[keysUsed].first = key;
		data[keysUsed++].second = key;
	}

	else if (keysUsed == MAX_DNODE_KEYS) {//split 
		maxNode = new dNode();
		for (size_t i = 0; i < MIN_DNODE_KEYS; ++i) {
			maxNode->data[i] = data[i + MIN_DNODE_KEYS];
		}
		maxNode->keysUsed = keysUsed = MIN_DNODE_KEYS;
		if (key < maxNode->data[0].first) //determine which of the two nodes to insert into
			this->Insert(key);
		else
			maxNode->Insert(key);
		if (nextNode == nullptr)
			nextNode = maxNode;
		else {
			maxNode->nextNode = nextNode;
			nextNode = maxNode;
		}
	}
	else if (data[keysUsed - 1].first < key) { //append to next free space
		data[keysUsed].first = key;
		data[keysUsed++].second = key;
	}

	//PrintDataNode(this);
	return maxNode;
}

//removes and returns the first element of keys and re-arranges keys.


void dNode::NodeDump(dNode* node, size_t level)
{
	string tabs(level * 2, ' ');
	cout << tabs << "[ ";
	int i = 0;
	try {
		for (; i < node->keysUsed; ++i) {
			cout << node->data[i].first;
			if (i != node->keysUsed - 1)
				cout << ", ";
		}
	}
	catch (...) {
		ostringstream err;
		err << "Error in dNode::NodeDump\n";
		err << "an exception was thrown while trying to access node->data[" << i << "].\n";
		throw std::exception(err.str().c_str());
	}
	cout << " ]\n";
}


bool dNode::iNodeTest(int i) {
	//if (i == 120)
	//	cout << "";
	for (int idx = 0; idx < keysUsed; ++idx) {
		if (i == data[idx].first) {
			return true;
		}
	}
	return false;
}

int dNode::DepthFirstLeafCheck(int key)
{
	for (int i = 0; i < keysUsed; ++i) {
		if (data[i].first <= key) {
			std::ostringstream err;
			err << "Error: DepthFirstLeafCheck - Leaf nodes are not in order.\n";
			err << "Test key: " << key << ".  Checked against " << data[i].first << "\n";
			PrintDataNode(this, err);
			if (nextNode != nullptr)
				PrintDataNode(nextNode, err);
			throw std::runtime_error(err.str());
		}
		key = data[i].first;
	}
	return key;
}

void dNode::Remove(Node * currentNode, int key, size_t currentLevel, Node * leftNeighbor, Node * rightNeighbor, Node * leftAnchor, Node * rightAnchor, size_t leftAnchorLevel, size_t rightAnchorLevel)
{
	//if (key == 3701)
	//	cout << ""; 
	assert(keysUsed != 0);
	assert(currentLevel == 0 || currentLevel != 0 && (keysUsed >= MIN_DNODE_KEYS && keysUsed <= MAX_DNODE_KEYS));
	bool found = false;
	for (int i = 0; i < keysUsed; ++i) {
		if (data[i].first == key) {
			found = true;
			for (int j = i; j < keysUsed - 1; ++j) {//shift data left one space
				data[j] = data[j + 1];
			}
			--keysUsed;
			break;
		}
	}
	if (!found)
		cout << "dNode::Remove : key " << key << " not found.\n";
	if (keysUsed < MIN_DNODE_KEYS && currentLevel != 0) { //need to shift or merge
		
		dNode* rightCast = static_cast<dNode*>(rightNeighbor); //easier to work with rather than always casting
		dNode* leftCast = static_cast<dNode*>(leftNeighbor);
		iNode* rightAnchCast = static_cast<iNode*>(rightAnchor);
		iNode* leftAnchCast = static_cast<iNode*>(leftAnchor);
		//shift LR
		if (leftNeighbor && leftCast->keysUsed > MIN_DNODE_KEYS && (!rightNeighbor || rightNeighbor && (rightCast->keysUsed < leftCast->keysUsed || leftAnchorLevel > rightAnchorLevel))) { 
			cout << "Leaf node shift LR...\n";
			for (int i = keysUsed; i > 0; --i)
				data[i] = data[i-1];  //shuffle current data right one space
			data[0] = leftCast->data[leftCast->keysUsed - 1];
			++keysUsed;
			--(leftCast->keysUsed);

			//find and fix left anchor pivot key
			int pivot = iNode::findAnchorPivot(leftAnchCast, key);
			if (key < leftAnchCast->keys[pivot]) //most common case
				--pivot;
			leftAnchCast->keys[pivot] = data[0].first;
			cout << "Anchor index: " << pivot << "\n";
			cout << "Dumping parent node...\n";
			iNode::NodeDump(leftAnchCast, leftAnchorLevel);
		}
		//shift RL
		else if (rightNeighbor && rightCast->keysUsed > MIN_DNODE_KEYS && (!leftNeighbor || leftNeighbor && (leftCast->keysUsed < rightCast->keysUsed || rightAnchorLevel >= leftAnchorLevel))) { 
			cout << "Leaf node shift RL...\n";
			data[MIN_DNODE_KEYS - 1].first = data[MIN_DNODE_KEYS - 1].second = rightCast->data[0].first;
			++keysUsed;
			//now shuffle right neighbor keys left one space
			for (int i = 0; i < rightCast->keysUsed - 1; ++i)
				rightCast->data[i].first = rightCast->data[i].second = rightCast->data[i + 1].first;
			--(rightCast->keysUsed);
			
			//now find and fix right anchor pivot key
			int pivot = iNode::findAnchorPivot(rightAnchCast, key);
			cout << "Anchor index: " << pivot << "\n";
			rightAnchCast->keys[pivot] = rightNeighbor->GetMinMax();
			cout << "Dumping parent node...\n";
			iNode::NodeDump(rightAnchCast, rightAnchorLevel);
		}
		else if (leftNeighbor && (!rightNeighbor || leftAnchorLevel >= rightAnchorLevel)){//merge left
			cout << "leaf node merge left...\n";
 			assert(leftCast->keysUsed == MIN_DNODE_KEYS);
			for (int i = 0; i < keysUsed; ++i) //copy into left neighbor
				leftCast->data[i + MIN_DNODE_KEYS] = data[i];
			++(leftCast->keysUsed);
			//find and remove pivot key
			int pivot = iNode::findAnchorPivot(leftAnchCast, key);
			if (key < leftAnchCast->keys[pivot])
				--pivot;
			for (int j = pivot; j < leftAnchCast->keysUsed - 1; ++j) { //shift keys and pointers left one space, deleting the pivot 
				leftAnchCast->keys[j] = leftAnchCast->keys[j + 1];
				leftAnchCast->children[j + 1] = leftAnchCast->children[j + 2];
			}
		
			--(leftAnchCast->keysUsed);
			cout << "Merge Left completed.  Left anchor keys used is " << pivot << "\n";
			leftCast->nextNode = nextNode; //fix linked list
			delete this;
			if (leftAnchCast){
				cout << "Dumping parent node...\n";
				iNode::NodeDump(leftAnchCast, leftAnchorLevel);
			}
			
		}
		else {//merge right
			assert(rightCast->keysUsed == MIN_DNODE_KEYS);
			cout << "leaf node merge right...\n";
			for (int i = 0; i < MIN_DNODE_KEYS; ++i)//copy keys from right to current
				data[i+ keysUsed] = rightCast->data[i];
			keysUsed += MIN_DNODE_KEYS;
			int pivot = iNode::findAnchorPivot(rightAnchCast, key);
			cout << "pivot index is: " << pivot << "\n";
			--rightAnchCast->keysUsed;
			for (int j = pivot; j < rightAnchCast->keysUsed; ++j) { //shuffle anchor keys and pointers left one space
				rightAnchCast->keys[j] = rightAnchCast->keys[j + 1];
				rightAnchCast->children[j + 1] = rightAnchCast->children[j + 2];
			}
			nextNode = rightCast->nextNode;
			delete rightNeighbor;
			cout << "Dumping parent node...\n";
			iNode::NodeDump(rightAnchCast, rightAnchorLevel);

		}
	}
}


dNode::~dNode()
{
	//no member variables other than leafHead which would double delete.
}