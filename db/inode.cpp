#include "inode.h"

iNode::iNode() : Node(), minMax(nullptr) {
	for (size_t i = 0; i < MAX_INODE_KEYS + 1; ++i)
		children[i] = nullptr;
	keysUsed = 0;
}
int iNode::GetMinMax() {
	return *minMax;
}
//check children array for null pointers.  Throws an exception.
void checkPointers(iNode* node) {
	for (int i = 0; i < node->keysUsed + 1; ++i)
		if (node->children[i] == nullptr){
			std::ostringstream out;
			out << "Error: iNode has a key with an invalid pointer.\n";
			out << "(";
			for (int j = 0; j < node->keysUsed + 1; ++j) {
				if (j == i)
					out << " nullptr ";
				else
					out << "*";
				if (j = !node->keysUsed)
					out << node->keys[j];
				out << (j == node->keysUsed - 1) ? "*)" : ",";
			}
			out << "\n";
			throw std::domain_error(out.str());
		}
}
Node* iNode::Insert(int key) {
	//if (key == 10513)
	//	cout << "";
	Node* primeNode = nullptr;
	iNode* currentLevelMaxNode = nullptr;
	int i;
	bool insertSuccess = false;
	for (i = 0; i < keysUsed; ++i) { //find where to insert
		if (key < keys[i]) {
			primeNode = children[i]->Insert(key);
			insertSuccess = true;
			break;
		}
		else if (key == keys[i]) {
			primeNode = children[i + 1]->Insert(key);
			insertSuccess = true;
			++i; //need this for reordering logic below
			break;
		}
	}
	if (!insertSuccess)  //case key belongs in trailing pointer
		primeNode = children[keysUsed]->Insert(key);

	if (primeNode != nullptr) { //split occured below this level
		cout << "A split occured below this iNode level.\n";
		if (keysUsed < MAX_INODE_KEYS) { //current iNode has room
			for (int idx = keysUsed; idx != i; --idx) {
				keys[idx] = keys[idx - 1];
				children[idx + 1] = children[idx];
			}
			keys[i] = primeNode->GetMinMax();
			children[i + 1] = primeNode;
			++(keysUsed);
		}
		else { //must split this node
			cout << "iNode splitting...\n";
			cout << "key: " << key << ".  keys[MIN_INODE_KEYS - 1]: " << keys[MIN_INODE_KEYS - 1] << "\n";
			currentLevelMaxNode = new iNode();
			if (currentLevelMaxNode == nullptr) //this is old behavior and should not happen.  Should instead throw bad_alloc
				throw std::exception("Error: Out of memory\n"); //had a problem occur which seems to indicate a nullptr was returned after a call to new

			//if (key == 18716)
			//	cout << "";
			if (key < keys[MIN_INODE_KEYS - 1]) { //uneven split
				cout << "iNode uneven split\n";
				currentLevelMaxNode->minMax = new int(keys[MIN_INODE_KEYS - 1]); //this is retrieved with getMinMax() by this node's parent
				for (int idx = 0; idx < MIN_INODE_KEYS; ++idx) { //copy keys and pointers to max node
					currentLevelMaxNode->keys[idx] = keys[idx + MIN_INODE_KEYS];
					currentLevelMaxNode->children[idx] = children[idx + MIN_INODE_KEYS];
				}
				currentLevelMaxNode->children[MIN_INODE_KEYS] = children[MAX_INODE_KEYS]; //assign max node trailing pointer

				for (int idx = MIN_INODE_KEYS; idx != i + 1; --idx) { //shift indexes and pointers of min node
					keys[idx - 1] = keys[idx - 2];
					children[idx] = children[idx - 1];
				}
				children[i + 1] = primeNode;
				keys[i] = primeNode->GetMinMax();
			}
			else { //even split
				cout << "iNode even split\n";
				   //if (key == 24726)
				   //cout << "";
				if (i == MIN_INODE_KEYS) { //split occured on the middle pointer.
					cout << "Split occured on middle pointer.\n";
					currentLevelMaxNode->minMax = new int(primeNode->GetMinMax());
					for (int j = 0; j < MIN_INODE_KEYS; j++) { //copy keys and pointers to the right.
						currentLevelMaxNode->keys[j] = keys[j + MIN_INODE_KEYS];
						currentLevelMaxNode->children[j + 1] = children[j + 1 + MIN_INODE_KEYS];
					}
					currentLevelMaxNode->children[0] = primeNode;
				}
				else { //split occured somewhere right of the middle pointer
					//if (key == 19627)
					//	cout << "";
					cout << "split occured somewhere right of the middle pointer.\n";
					currentLevelMaxNode->minMax = new int(keys[MIN_INODE_KEYS]);
					for (int j = 0; j < MIN_INODE_KEYS - 1; ++j) {//copy keys and pointers right, exclude what would become be the minMax
						currentLevelMaxNode->keys[j] = keys[j + MIN_INODE_KEYS + 1];
						currentLevelMaxNode->children[j] = children[j + MIN_INODE_KEYS + 1];
					}
					currentLevelMaxNode->children[MIN_INODE_KEYS - 1] = children[MAX_INODE_KEYS];
					int mm = primeNode->GetMinMax();
					//find appropriate place to insert the new key & pointer
					for (int j = MIN_INODE_KEYS - 1; ;--j) {
						if (j == 0) {
							currentLevelMaxNode->keys[0] = mm;
							currentLevelMaxNode->children[1] = primeNode;
							break;
						}
						else if (mm < currentLevelMaxNode->keys[j - 1]) {
							currentLevelMaxNode->keys[j] = currentLevelMaxNode->keys[j - 1]; //shift key right
							currentLevelMaxNode->children[j + 1] = currentLevelMaxNode->children[j]; //shift pointer right
						}
						else {
							currentLevelMaxNode->keys[j] = mm;
							currentLevelMaxNode->children[j + 1] = primeNode;
							break;
						}
						assert(j >= 0);
					}
				}
			}
			currentLevelMaxNode->keysUsed = keysUsed = MIN_INODE_KEYS;
		}
	}
	checkPointers(this);
	if (currentLevelMaxNode != nullptr)
		checkPointers(currentLevelMaxNode);
	
	return currentLevelMaxNode;
}


void iNode::NodeDump(iNode* node, size_t level) {
	string indent(level * 2, ' ');
	cout << indent;
	cout << "(  *";
	for (int i = 0; i < node->keysUsed; ++i)
		cout << ", " << node->keys[i] << " *";
	cout << ")\n";
	++level;
	int i = 0;
	try {
		for (; i < node->keysUsed + 1; ++i)
			node->children[i]->NodeDump(level);
	}
	catch (std::exception& e) {
		throw e; //catch and release
	}
	catch (...) {
		ostringstream out;
		out << "Error: attempted to reference a bad pointer.\n";
		out << "Triggered when attempting to access children[" << i << "]\n";
		out << "Keys: " << node->keysUsed << "\n";
		out << "* ";
		for (int i = 0; i < MAX_INODE_KEYS; ++i)
			out << node->keys[i] << " * ";
		out << "\n";
		for (int i = 0; i < MAX_INODE_KEYS + 1; ++i) 
			out << node->children[i] << " ";
		throw std::exception(out.str().c_str());
	}
}
bool iNode::iNodeTest(int i) {
	for (int idx = 0; idx < keysUsed; ++idx) {
		if (i < keys[idx]) {
			return children[idx]->iNodeTest(i);
		}
	}
	return children[keysUsed]->iNodeTest(i);
}
int iNode::DepthFirstLeafCheck(int key)
{
	//cout << keys[0] << "\n";
	//if (keys[0] == 17069 && keys[1] == 16881)
	//	cout << "";
	for (int i = 0; i < keysUsed - 1; ++i) {
		if (keys[i] >= keys[i + 1]) {//error!  index keys are out of order
			ostringstream err;
			err << "Error: iNode keys are not in the correct order.\n";
			err << "( ";
			for (int j = 0; j < keysUsed; ++j)
				err << keys[j] << " ";
			err << ")";
			throw std::logic_error(err.str());
		}
	}
	for (int i = 0; i < keysUsed - 1; ++i) {
		key = children[i]->DepthFirstLeafCheck(key);
		if (key >= keys[i]) {
			ostringstream err;
			err << "Error: key returned from leaf node is greater or equal to key in index node.\n";
			err << "Key returned from leaf node: " << key << ".  Key in index node: " << keys[i] << "\n";
			err << "iNode key index: " << i << "\n";
			throw std::logic_error(err.str());
		}
	}
	key = children[keysUsed]->DepthFirstLeafCheck(key); //check trailing pointer
	if (key < keys[keysUsed - 1]) {
		ostringstream err;
		err << "Error: key returned from trailing pointer is greater than last key in index node.\n";
		err << "Last iNode key: " << keys[keysUsed - 1] << ".  Key returned from leaf node: " << key << "\n";
		throw std::logic_error(err.str());
	}
	return key;
}
//returns the index of the anchor's pivot key
int iNode::findAnchorPivot(iNode* anchor, int key) {
	for (int i = 0; i < anchor->keysUsed; ++i)
		if (key < anchor->keys[i])
			return i;
	return anchor->keysUsed -1; //tail case
}
void iNode::Remove(Node * currentNode, int key, size_t currentLevel, Node * leftNeighbor, Node * rightNeighbor, Node * leftAnchor, Node * rightAnchor, size_t leftAnchorLevel, size_t rightAnchorLevel)
{
	assert(keysUsed > 0);
	assert(currentLevel == 0 || keysUsed >= MIN_INODE_KEYS && keysUsed <= MAX_INODE_KEYS);
	Node* nextLeftNeighbor;
	Node* nextRightNeighbor;
	Node* nextLeftAnchor;
	Node* nextRightAnchor;
	size_t nextLeftALevel;
	size_t nextRightALevel;
	int pivot = 0;
	for (; pivot < keysUsed; ++pivot)
		if (key < keys[pivot]) //key found
			break;
	iNode* rightNCast = static_cast<iNode*>(rightNeighbor);
	iNode* leftNCast = static_cast<iNode*>(leftNeighbor);
	if (pivot == 0) { //next node is left most node
		nextLeftAnchor = leftAnchor;
		nextLeftALevel = leftAnchorLevel;
		nextRightAnchor = this;
		nextRightALevel = currentLevel;
		nextLeftNeighbor = leftNeighbor == nullptr ? nullptr : leftNCast->children[leftNCast->keysUsed];
		nextRightNeighbor = children[pivot + 1];
	}
	else if (pivot == keysUsed) { //next node is right most node
		nextLeftAnchor = this;
		nextLeftALevel = currentLevel;
		nextRightAnchor = rightAnchor;
		nextRightALevel = rightAnchorLevel;
		nextLeftNeighbor = children[pivot - 1];
		nextRightNeighbor = rightNeighbor == nullptr ? nullptr : rightNCast->children[0];
	}
	else { //next node is a middle node
		nextLeftAnchor = this;
		nextLeftALevel = currentLevel;
		nextRightAnchor = this;
		nextRightALevel = currentLevel;
		nextLeftNeighbor = children[pivot - 1];
		nextRightNeighbor = children[pivot + 1];
	}
	children[pivot]->Remove(nullptr, key, currentLevel + 1, nextLeftNeighbor, nextRightNeighbor, nextLeftAnchor, nextRightAnchor, nextLeftALevel, nextRightALevel);
	//now does this node need shifting or merging?
	
	if (currentLevel != 0 && keysUsed < MIN_INODE_KEYS) { //do I need to merge or shfit?
		iNode* leftCast = static_cast<iNode*>(leftNeighbor);
		iNode* rightCast = static_cast<iNode*>(rightNeighbor);
		iNode* rightACast = static_cast<iNode*>(rightAnchor);
		iNode* leftACast = static_cast<iNode*>(leftAnchor);
		
		//shift LR
		if (leftNeighbor && leftCast->keysUsed > MIN_INODE_KEYS && (!rightNeighbor || rightNeighbor && (rightCast->keysUsed < leftCast->keysUsed || leftAnchorLevel > rightAnchorLevel))) {
			cout << "iNode shift LR...\n";
			assert(leftCast->keysUsed > MIN_INODE_KEYS && keysUsed < MIN_INODE_KEYS);
			for (int i = MIN_INODE_KEYS - 1; i > 0; --i) //shift keys right one space
				keys[i] = keys[i - 1];
			for (int i = MIN_INODE_KEYS; i >= 0; --i) //shift pointers
				children[i + 1] = children[i];

			//rotate keys clockwise
			pivot = findAnchorPivot(leftACast, key);
			if (key < leftACast->keys[pivot]) //most common case
				--pivot;
			cout << "anchor pivot index: " << pivot << "\n";
			keys[0] = leftACast->keys[pivot];
			leftACast->keys[pivot] = leftCast->keys[leftCast->keysUsed - 1];

			children[0] = leftCast->children[leftCast->keysUsed]; //move pointer from left to right
			leftCast->children[leftCast->keysUsed--] = nullptr; //just to be safe
			++keysUsed;
			cout << "Dumping parent node...\n";
			iNode::NodeDump(leftACast, leftAnchorLevel);
		}
		//shift RL
		else if (rightNeighbor && rightCast->keysUsed > MIN_DNODE_KEYS && (!leftNeighbor || leftNeighbor && (leftCast->keysUsed < rightCast->keysUsed || rightAnchorLevel >= leftAnchorLevel))) {
			cout << "iNode Shift RL...\n";
			assert(keysUsed < MIN_INODE_KEYS && rightCast->keysUsed > MIN_INODE_KEYS);
			int anchorPivot = findAnchorPivot(rightACast, key);
			cout << "anchor pivot index: " << anchorPivot << "\n";
			//rotate keys counter-clockwise
			keys[keysUsed] = rightACast->keys[anchorPivot]; //copy anchorPivot to current
			children[keysUsed + 1] = rightCast->children[0]; //move ponter from right to current
			++keysUsed;
			rightACast->keys[anchorPivot] = rightCast->keys[0]; //copy key from right to anchorPivot
			for (int i = 0; i < rightCast->keysUsed - 1; ++i) //shift right neighbor keys left one space
				rightCast->keys[i] = rightCast->keys[i + 1];
			for (int i = 0; i < rightCast->keysUsed; ++i) //shift right neighbor pointers left one space
				rightCast->children[i] = rightCast->children[i + 1];
			--rightCast->keysUsed;
			rightCast->children[rightCast->keysUsed + 1] = nullptr; //house keeping
			cout << "Dumping parent node...\n";
			iNode::NodeDump(rightACast, rightAnchorLevel);
		}
		//merge left
		else if (leftNeighbor && (!rightNeighbor || leftAnchorLevel >= rightAnchorLevel) ) {
			cout << "iNode merge left...\n";
			int anchorPivot = findAnchorPivot(leftACast, key);
			if (key < leftACast->keys[anchorPivot])
				--anchorPivot;
			cout << "anchor pivot index: " << anchorPivot << "\n";
			leftCast->keys[MIN_INODE_KEYS] = leftACast->keys[anchorPivot];
			++(leftCast->keysUsed);
			for (int i = 0; i < keysUsed; ++i)  //copy keys to left node
				leftCast->keys[leftCast->keysUsed + i] = keys[i];
			for (int i = 0; i < keysUsed + 1; ++i) //copy pointers to left node
				leftCast->children[leftCast->keysUsed + i] = children[i];
			leftCast->keysUsed = MAX_INODE_KEYS;
			delete this;
			for (int i = anchorPivot; i < leftACast->keysUsed - 1; ++i) //shift anchor keys left one
				leftACast->keys[i] = leftACast->keys[i + 1];
			for (int i = anchorPivot + 1; i < leftACast->keysUsed; ++i) //shift anchor pointers left one
				leftACast->children[i] = leftACast->children[i + 1];
			--(leftACast->keysUsed);
			cout << "Dumping parent node...\n";
			iNode::NodeDump(leftACast, leftAnchorLevel);
		}
		//merge right
		else {
			cout << "iNode merge right...\n";
			int anchorPivot = findAnchorPivot(rightACast, key);
			cout << "anchor pivot index: " << anchorPivot << "\n";
			keys[MIN_INODE_KEYS-1] = rightACast->keys[anchorPivot]; //copy anchorPivot key to current
			++keysUsed;
			--(rightACast->keysUsed); 
			for (int i = anchorPivot; i < rightACast->keysUsed; ++i)  //shift anchor keys left one (deletes key)
				rightACast->keys[i] = rightACast->keys[i + 1];
			for (int i = anchorPivot + 1; i < rightACast->keysUsed + 1; ++i) //shift anchor pointers left one
				rightACast->children[i] = rightACast->children[i + 1];
			for (int i = keysUsed; i < MAX_INODE_KEYS; ++i) //copy keys from right neighbor to current
				keys[i] = rightCast->keys[i - MIN_INODE_KEYS];
			for (int i = keysUsed; i < MAX_INODE_KEYS + 1; ++i) //copy pointers from right neighbor to current
				children[i] = rightCast->children[i - MIN_INODE_KEYS];
			keysUsed = MAX_INODE_KEYS;
			delete rightNeighbor;
			cout << "Dumping parent node...\n";
			iNode::NodeDump(rightACast, rightAnchorLevel);
		}
	}
}

void iNode::Rebalance(Node * currentNode, int key, size_t currentLevel, Node * leftNeighbor, Node * rightNeighbor, Node * leftAnchor, Node * rightAnchor, size_t leftAnchorLevel, size_t rightAnchorLevel)
{
	throw notImplementedException();
}

iNode::~iNode()
{
	delete minMax;
}