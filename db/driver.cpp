#include <ctime>
#include <memory>
#include <iostream>
#include <map>
#include <limits>
#include <cstdlib>
#include <fstream>
#include "db.h"
#include "node.h"
#include <vector>
using std::vector;
using std::rand;
using std::srand;
using std::map;
using std::cout;
using std::cin;
using std::endl;
using std::cin;

void PrintMenu() {
	
	
	cout << "\n0 - Exit" "\n";
	cout << "1- Create Tree" "\n";
	cout << "2- Auto Create Tree" "\n";
	cout << "3- Delete Specific Keys" "\n";
	cout << "4- Auto create and delete tree\n";
	cout << "5- Random insertion stress test\n";
	cout << "6- Random insertion + delete stress test\n";
}

void clear_cin(std::istream& c) {
	cout << "Bad selection" "\n";
	cin.clear();
	cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int main() {
	

	std::unique_ptr<DB> db = std::unique_ptr<DB>(new DB());
	size_t menuChoice = 255;
	PrintMenu();
	cin >> menuChoice;
	while (menuChoice != 0) {
		if (!cin)
			clear_cin(cin);
		else if (menuChoice == 1) {
			if (!db)
				db = std::unique_ptr<DB>(new DB());
			int keyToInsert = 0;
			cout << "Begin inserting keys. -99 to quit\n";
			while (keyToInsert != -99) {
				if (!(cin >> keyToInsert))
					clear_cin(cin);
				else {
					if (keyToInsert == -99) break;
					db->Insert(keyToInsert);
					db->NodeDump();
					db->LeafDump();
					try{
						db->LeafCheck();
					}
					catch (std::exception& e){
						cout << e.what() << endl;
						cin.get();
					}
					catch (string& s) {
						cout << s << endl;
						cin.get();
					}
				}
			} 
		}
		else if (menuChoice == 2) {
			if (!db)
				db = std::unique_ptr<DB>(new DB());
			cout << "Create how many entries? ";
			
			int howMany = 0;
			while (!(cin >> howMany))
				clear_cin(cin);
			cout << "\n";
			for (int i = 10; i <= howMany * 10; i += 10) {
				cout << "Inserting " << i << "\n";
				db->Insert(i);
			}
			//db->NodeDump();
			//db->LeafDump();
			/*if (db->iNodeTest(i) == false) {
				cout << "\n*** Index " << i << " not found in tree.***\n";
				system("PAUSE");
			}
			try{
				db->LeafCheck();
			}
			catch (exception& e){
				cout << e.what();
				return -1;
			}*/
			db->NodeDump();
		}
		
		else if (menuChoice == 3) {
			cout << "Enter keys.  -99 to quit\n";
			int key = 0;
			while (key != -99) {
				while (!(cin >> key))
					clear_cin(cin);
				if (key == -99) break;
				db->Remove(key);
				db->NodeDump();
			}
		}
		else if (menuChoice == 4) {
			db = std::unique_ptr<DB>(new DB());
			cout << "How many keys to insert and then delete? ";
			int howMany = 0;
			while (!(cin >> howMany))
				clear_cin(cin);
			cout << "\n";
			vector<int> keys;
			time_t seed = time(NULL);
			srand(static_cast<size_t>(seed));
			//srand(1457379175);
			cout << "seed is " << seed << "\n";
			int toInsert = 0;
			try {
				for (int i = 0; i < howMany; ++i) {
					toInsert = rand();
					keys.push_back(toInsert);
					db->Insert(toInsert);
				}
				db->NodeDump();
				db->LeafCheck();
				cout << "begin removing keys...\n";
				int toRemove = 0;
				for (size_t i = 0; keys.size() > 0; ) {
					toRemove = keys[0];
					cout << "Removing " << toRemove << endl;
					//if (keys.front() == 32246)
					//	cout << "";
					db->Remove(toRemove);
					if (db->iNodeTest(toRemove)) {
						cout << "Error: key " << toRemove << " is still in tree after attempted delete.\n";
						system("PAUSE");
					}
					db->NodeDump();
					db->LeafCheck();
					db->LeafDump();
					keys.erase(keys.begin());
				}
				//cout << "vector size is " << keys.size() << endl;
			}
			catch (std::bad_alloc& e) {
				cout << "Error: out of memory.\n";
				cout << e.what() << "\n";
				system("PAUSE");
			}
			catch (std::exception& e) {
				cout << "Error: exception caught: \n";
				cout << e.what() << endl;
				system("PAUSE");
			}
			catch (...) {
				cout << "Error: General exception catch (...) triggered.\n";
				system("PAUSE");
			}
		}
	
		else if(menuChoice == 5) {
			
			int toInsert = 0;
			//auto seed = 1456114831;
			auto seed = time(NULL);
			while (true) { //run until crash for lack of memory
				srand(static_cast<size_t>(seed)); //cast to avoid compiler warning
				cout << "Random seed is " << seed << "\n";
				db = std::unique_ptr<DB>(new DB());
				for (int i = 0; i < RAND_MAX / 2; ++i) {
					toInsert = rand();
					cout << "Inserting " << toInsert << "\n";
					if (toInsert == 10513)
						cout << "";
					try {
						db->Insert(toInsert);
						db->NodeDump();
						db->LeafDump();
						if (!db->iNodeTest(toInsert)) {
							cout << "iNodeTest failed\n";
							system("PAUSE");
							db->LeafCheck();
							db->DepthFirstLeafCheck();
						}
					}
					catch (std::bad_alloc& ba) {
						cout << "bad_alloc caught: " << ba.what() << "\n";
						system("PAUSE");
					}
					catch (string& s) {
						cout << "exception cought...\n";
						cout << "(string s)\n";
						cout << s;
						system("PAUSE");
					}
					catch (std::exception& e) {
						cout << e.what() << "\n";
						system("PAUSE");
					}
					catch (...) {
						cout << "exception cought...\n";
						cout << "default exception catch\n";
						cout << "Likely a segfault/bad pointer\n";
						system("PAUSE");
					}
				}
			}
		}
		else if (menuChoice == 7) { //skip print statements and checks to get to a known error quicker
			srand(1456121195);
			db = std::unique_ptr<DB>(new DB());
			int toInsert;
			for (int i = 0; i < 1000; ++i) {
				toInsert = rand();
				db->Insert(toInsert);
				}
			
			while (true) { //run indefinately
				vector<int> buffer;
				int removeIndex;
				while (true)
					toInsert = rand();
				try {
					for (int i = 0; i < 10; ++i) {
						buffer.push_back(toInsert);
						if (toInsert == 11930) {
							cout << ""; //break point
							db->NodeDump();
						}
						db->Insert(toInsert);
					}
					while (buffer.size() > 0) {
						removeIndex = rand() % buffer.size();
						if (buffer[removeIndex] == 11930){
							cout << "";//break point
							db->NodeDump();
						}
						db->Remove(buffer[removeIndex]);
						buffer.erase(buffer.begin() + removeIndex);
					}
				}
				catch (exception& e) {
					cout << e.what() << "\n";
					cin.get();
				}
				catch (...) {
					cout << "Caught some exception (...)\n";
					cin.get();
				}
			}
		}
		else if (menuChoice == 6) {
			cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //sometimes cin has an extra 'enter' in the stream
			auto seed = time(NULL);
			//auto seed = 1456397256;
			srand(static_cast<size_t>(seed)); 
			cout << "Random seed is " << seed << "\n";
			db = std::unique_ptr<DB>(new DB());
			int toInsert;
			cout << "start off with a DB of about 1000 entries\n";
			for (int i = 0; i < 1000; ++i) {
				toInsert = rand();
				cout << "Inserting " << toInsert << "\n";
				try{
				db->Insert(toInsert);
				db->iNodeTest(toInsert);
				db->LeafCheck();
				}
				catch (exception& e) {
					cout << e.what() << "\n";
					cin.get();
				}
				catch (...) {
					cout << "Caught some exception (...)\n";
					cin.get();
				}
			}
			while (true) { //run indefinately
				vector<int> buffer;
				int removeIndex;
				try {
					for (int i = 0; i < 10; ++i) {
						buffer.push_back(toInsert = rand());
						cout << "Inserting " << toInsert << "\n";
						db->Insert(toInsert);
						db->NodeDump();
						db->DepthFirstLeafCheck();
						db->LeafCheck();
						cout.flush();
					}
					while (buffer.size() > 0){
						removeIndex = rand() % buffer.size();
						//if (buffer[removeIndex] == 16902)
						//	cout << ""; //break point
						cout << "Removing " << buffer[removeIndex] << "\n";
						db->Remove(buffer[removeIndex]);
						db->NodeDump();
						db->DepthFirstLeafCheck();
						db->LeafCheck();
						buffer.erase(buffer.begin() + removeIndex);
						cout.flush();
					}
				}
				catch (std::bad_alloc& e) {
					cout << "Error: out of memory.\n" << e.what() << "\n";
				}
				catch (exception& e) {
					cout << e.what() << "\n";
					cout.flush();
					cin.get();
				}
				catch (...) {
					cout << "Caught some exception (...)\n";
					cout << "Likely attempted to access a deleted pointer\n";
					cout.flush();
					
					cin.get();
				}
			}
		}

		PrintMenu();
		cin >> menuChoice;
	}
	std::cin.get();
}