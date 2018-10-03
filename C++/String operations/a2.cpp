///////////////////////////////////////////////////////////////////////
// File Name:      a2.cpp
//
// Author:         Zihan Wang
// CS email:       zian@cs.wisc.edu
// 
// Description:   ABC
//
// Sources:       
//
// URL(s) of sources:           
///////////////////////////////////////////////////////////////////////
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <iterator>
using namespace std;

int main() {
	// get input
	std::cout << "Enter the name of the first file: ";
	std::string userFileName1;
	std::cin >> userFileName1;
	ifstream inFile(userFileName1);
	while (!inFile.is_open()) {
		std::cout <<"Input file " << userFileName1 << " is NOT found. Please try again." 
		<< std::endl;
		std::cout << "Enter the name of the first file: ";
		std::cin >> userFileName1;
		inFile.open(userFileName1);
	}
	std::cout << "Enter the name of the second file: ";
	std::string userFileName2;
	cin >> userFileName2;
	ifstream inFile2nd(userFileName2);
	while (!inFile2nd.is_open()) {
		std::cout <<"Input file " << userFileName2 << " is NOT found. Please try again."
		 << std::endl;
                std::cout << "Enter the name of the second file: ";
                cin >> userFileName2;
                inFile2nd.open(userFileName2);
	}
	// get data into the vars
	vector<string> vNames1, vNames2;
	std::string sName1, sName2;
	while (true) {
		getline(inFile, sName1);
		if(inFile.fail()) {
			break;
		}
		vNames1.push_back(sName1);
		sName1.clear();
	}
	while (true) {
		getline(inFile2nd, sName2);
		if(inFile2nd.fail()) {
			break;
		}
		vNames2.push_back(sName2);
		sName2.clear();
	}
	// output starts
	std::cout << "\nNumber of recipes in " << userFileName1 << " = ";
	std::cout << vNames1.size() << std::endl;
	std::cout << "Number of recipes in " << userFileName2 << " = ";
	std::cout << vNames2.size() << std::endl;
	sort(vNames1.begin(), vNames1.end());
	sort(vNames2.begin(), vNames2.end());
	// get intersection and union
	std::vector<string> v_intersection;
	set_intersection(vNames1.begin(), vNames1.end(), 
	vNames2.begin(), vNames2.end(), std::back_inserter(v_intersection));
	
	std::vector<string> v_union;
	set_union(vNames1.begin(), vNames1.end(),
	 vNames2.begin(), vNames2.end(), std::back_inserter(v_union));
	
	std::cout << "Number of recipes that are present in BOTH " 
	<< userFileName1 << " AND " << userFileName2 << " = " << v_intersection.size() 
	<< std::endl;
	std::cout << "Number of recipes that are in EITHER " 
	<< userFileName1 << " OR " << userFileName2 << " = " << v_union.size() << std::endl;
	
	if (v_intersection.size() > 0) {
		std::cout << "\nList of recipes that are present in BOTH " << userFileName1 
		<< " AND " << userFileName2 << ":" << std::endl;
		for (auto it = v_intersection.begin(); it != v_intersection.end(); ++it) {
			std::cout << *it << std::endl;
		}
	}
	
	ofstream intersectionOut("intersection.txt");
	ofstream unionOut("union.txt");
	// output to the files
	cout << "\nThe set intersection of the two files was written to a"
	<< " file named intersection.txt\nThe set union of the two files"
	<< " was written to a file named union.txt" << endl;
	
	if (v_intersection.size() > 0) {
		for (auto it = v_intersection.begin(); it != v_intersection.end(); ++it) {
			intersectionOut << *it << endl;
		}
	}
	if (v_union.size() > 0) {
		for (auto it = v_union.begin(); it != v_union.end(); ++it) {
			unionOut << *it << endl;
		}
	}
	// normal exit
	return 0;
}
