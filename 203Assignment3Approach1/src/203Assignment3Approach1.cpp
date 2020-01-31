#include <iostream>
#include <vector>
#include <fstream>
#include <ctime>
using namespace std;

//function headers
void displayMenu();
void readFile(int* matrix[], int numberOfVillages);
void generateShortestDistancesUsingFloyd(int* matrix[], int numberOfVillages);
void query(int* matrix[], int* next[]);
bool isTraversalPossible(int* matrix[], int* next[], int source,
		int destination, int capacity);
void findPath(int* matrix[], int* next[], int source, int destination,
		int maxCapacity, int availableQuantity, vector<int>& orderOfVisit,
		vector<int>& costsOfVisit, vector<int>& villagesToRefillAt);

//function to start the program. Calls displayMenu() to display a menu to the user
void startProgram() {
	displayMenu();
}

//function to display a menu to the user asking him which file to read from
void displayMenu() {
	cout
			<< "Press\n1. to read from Shortest-to-post-100\n2. to read from the Shortest-to-post-300 text file: "
			<< endl;
	int choice;
	cin >> choice;
	int numberOfVillages;
	switch (choice) {
	case 1:
		numberOfVillages = 100;
		break;
	case 2:
		numberOfVillages = 300;
		break;
	default:
		cout << "Invalid choice! Program terminating." << endl;
		exit(1);
	}

	//matrix to hold costs of moving from one village to another
	int* matrix[numberOfVillages];
	readFile(matrix, numberOfVillages);

	for (int i = 0; i < numberOfVillages; i++) {
		delete[] matrix[i];
	}

}

//function to read data from the chosen file into the matrix holding costs of traversal from
//one village to another
void readFile(int* matrix[], int numberOfVillages) {
	ifstream inFile;
	string fileName;
	if (numberOfVillages == 100) {
		fileName = "Shortest-to-post-100";
	} else {
		fileName = "Shortest-to-post-300";
	}

	inFile.open(fileName);

	if (!inFile) {
		cerr << "File not found! Program terminating" << endl;
	}

	for (int i = 0; i < numberOfVillages; i++) {
		matrix[i] = new int[numberOfVillages];
	}

	for (int counter = 0; counter < 3 * numberOfVillages * numberOfVillages;
			counter++) {
		int index1;
		int index2;
		inFile >> index1 >> index2;
		inFile >> matrix[index1][index2];

		//setting negative distances to 2
		if (matrix[index1][index2] < 0) {
			matrix[index1][index2] = 2;
		}

		//setting infinite distances to INT_MAX
		if (matrix[index1][index2] == 0) {
			matrix[index1][index2] = INT_MAX;
		}

		//setting diagonal elements to 0
		if (index1 == index2) {
			matrix[index1][index2] = 0;
		}

	}
	inFile.close();
	generateShortestDistancesUsingFloyd(matrix, numberOfVillages);
}

//function that updates the matrix with values for all-pairs shortest paths using Floyd's
//algorithm
void generateShortestDistancesUsingFloyd(int* matrix[], int numberOfVillages) {
	int* next[numberOfVillages];
	for (int i = 0; i < numberOfVillages; i++) {
		next[i] = new int[numberOfVillages];
	}
	for (int i = 0; i < numberOfVillages; i++) {
		for (int j = 0; j < numberOfVillages; j++) {
			next[i][j] = j;
		}
	}
	clock_t startTime = clock();
	for (int k = 0; k < numberOfVillages; k++) {
		for (int i = 0; i < numberOfVillages; i++) {
			if (k == i) {
				continue;
			}
			for (int j = 0; j < numberOfVillages; j++) {
				if (k == j || i == j) {
					continue;
				}

				int newCost = INT_MAX;
				if ((matrix[i][k] != INT_MAX && matrix[k][j] != INT_MAX)
						|| !(matrix[i][k] + matrix[k][j] < 0)) {
					newCost = matrix[i][k] + matrix[k][j];
				}
				if (newCost < matrix[i][j]) {
					matrix[i][j] = newCost;
					next[i][j] = next[i][k];
				}
			}
		}
	}
	printf("\nMatrix generation took time: %.5fs\n\n",
			(double) (clock() - startTime) / CLOCKS_PER_SEC);

	query(matrix, next);
	for (int i = 0; i < numberOfVillages; i++) {
		delete[] next[i];
	}
}

//function that asks the user for traversals to be queried
void query(int* matrix[], int* next[]) {
	int source;
	int destination;
	int capacity;

	cout << "Enter village 1: " << flush;
	cin >> source;
	cout << "Enter village 2: " << flush;
	cin >> destination;
	cout << "Enter bottle capacity: " << flush;
	cin >> capacity;

	bool isPossible = isTraversalPossible(matrix, next, source, destination,
			capacity);
	if (!isPossible) {
		cout << "You cannot move from village " << source << " to village "
				<< destination << " with a bottle of capacity " << capacity
				<< endl;
	} else {

		vector<int> orderOfVisit;
		vector<int> costsOfVisit;
		vector<int> villagesToRefillAt;
		orderOfVisit.push_back(source);
		findPath(matrix, next, source, destination, capacity, capacity,
				orderOfVisit, costsOfVisit, villagesToRefillAt);
		cout << "Path to be traversed: " << flush;
		for (auto o = orderOfVisit.begin(); o != orderOfVisit.end(); o++) {
			if (o != orderOfVisit.end() - 1)
				cout << *o << "->" << flush;
			else
				cout << *o << flush;
		}
		cout << "\nSuccessive costs of visit: " << flush;
		for (auto c = costsOfVisit.begin(); c != costsOfVisit.end(); c++) {
			if (c != costsOfVisit.end() - 1)
				cout << *c << "->" << flush;
			else
				cout << *c << flush;
		}
		cout << "\nVillages to refill bottle at: " << flush;
		if (villagesToRefillAt.empty()) {
			cout << "None" << endl;
		} else {
			for (auto v : villagesToRefillAt) {
				cout << v << " " << flush;
			}
			cout << endl;
		}
	}
}

//function that recursively checks if the traversal from the source to the destination can be
//performed using a bottle of provided capacity
bool isTraversalPossible(int* matrix[], int* next[], int source,
		int destination, int capacity) {

	if (next[source][destination] == destination) {
		if (matrix[source][destination]
				> capacity|| matrix[source][destination] == INT_MAX) {
			return false;
		}
		return true;
	} else {
		int intermediate = next[source][destination];
		if (matrix[source][intermediate] > capacity) {
			return false;
		}
		source = intermediate;
		return isTraversalPossible(matrix, next, source, destination, capacity);

	}

}

//function that recursively finds the actual path to be traversed in going from the source to
//the destination
void findPath(int* matrix[], int* next[], int source, int destination,
		int maxCapacity, int availableQuantity, vector<int>& orderOfVisit,
		vector<int>& costsOfVisit, vector<int>& villagesToRefillAt) {

	if (next[source][destination] == destination) {
		orderOfVisit.push_back(destination);
		if (matrix[source][destination] > availableQuantity) {
			availableQuantity = maxCapacity;
			villagesToRefillAt.push_back(source);
		}
		costsOfVisit.push_back(matrix[source][destination]);
	} else {
		int intermediate = next[source][destination];
		if (matrix[source][intermediate] > availableQuantity) {
			availableQuantity = maxCapacity;
			villagesToRefillAt.push_back(source);
		} else {
			availableQuantity = availableQuantity
					- matrix[source][intermediate];
		}
		orderOfVisit.push_back(intermediate);
		costsOfVisit.push_back(matrix[source][intermediate]);

		source = intermediate;
		findPath(matrix, next, source, destination, maxCapacity,
				availableQuantity, orderOfVisit, costsOfVisit,
				villagesToRefillAt);
	}
}

//the main program
int main() {

	//call to the function to start the program
	startProgram();

	return 0;
}

