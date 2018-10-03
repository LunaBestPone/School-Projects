///////////////////////////////////////////////////////////////////////////////
// File Name:	  processStudent.cpp
//
// Author:		 Zihan Wang
// CS email:	   zwang883@wisc.edu
//
// Description:	Methods to perform some processing on student objects.
//
// Sources:		<Sources, if any>
//
// URL(s) of sources:
//				 <URLs of your sources, if any>
///////////////////////////////////////////////////////////////////////////////

#include "processStudent.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>
#include "Student.hpp"
#include "GradStudent.hpp"
#include "UndergradStudent.hpp"



void fillStudents(std::istream &inFile,
				  std::vector<std::shared_ptr<Student>>& gstudentPtrs,
				  std::vector<std::shared_ptr<Student>>& ugstudentPtrs) {


	while(true) {
		std::string aLine;
		std::getline(inFile, aLine);
		if(inFile.fail()) {
			break;
		}
		std::stringstream ss(aLine);
		std::string item;
		std::vector<std::string> elements;
		while (std::getline(ss, item, ',')) {
			elements.push_back(item);
		}
		std::string name = elements[1];
		std::vector<double> assignmentsScore;
		for (int i =2; i <= 7; i++) {
			assignmentsScore.push_back(std::stod(elements[i]));
		}
		double projectScore = std::stod(elements[8]);
		std::string nine;
		std::string ten;
		nine = elements[9];
		ten = elements[10];
		if (elements[0] == "U") {
			std::shared_ptr<Student> ugs(new UndergradStudent(name, assignmentsScore, projectScore, nine, ten));
			ugstudentPtrs.push_back(ugs);
		   }
		else {
			std::shared_ptr<Student> gs(new GradStudent(name, assignmentsScore, projectScore, nine, ten));
			gstudentPtrs.push_back(gs);
		}
	}
}

void printStudents(std::vector<std::shared_ptr<Student>>& students) {

	for(auto ptr : students) {
		ptr->printDetails();
		std::cout << std::endl;
	}
}

void computeStatistics(std::vector<std::shared_ptr<Student>>& students) {



	// display the number of students (undergrad or grad)
	std::cout << "Number of students = " << students.size() << std::endl;
	// compute the mean of the total score.
	double meanTotal = 0l;
	double sumTotal =0l;
	for (auto ptr : students) {
		sumTotal += ptr->getTotal();
	}
	meanTotal = sumTotal/((double) students.size());
	std::cout << "The mean of the total score = " << meanTotal << std::endl;
	// sort and print the students based on their total.
	std::cout << "The sorted list of students (id, name, total, grade) in descending order of total:" << std::endl;
	std::sort(students.begin(), students.end(), [] (std::shared_ptr<Student>& x, std::shared_ptr<Student>& y) -> bool {return x->getTotal() > y->getTotal();});
	for (auto ptr : students) {
		std::cout << ptr->Student::getId() << ", ";
		std::cout << ptr->Student::getName() << ", ";
		std::cout << ptr->getTotal() << ", ";
		std::cout << ptr->getGrade() << std::endl;
	}
	std::cout << std::endl;
}
