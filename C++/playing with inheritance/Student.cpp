////////////////////////////////////////////////////////////////////////////////
// File Name:      Student.cpp
//
// Author:         Zihan Wang
// CS email:       zwang883@wisc.edu
//
// Description:    implementation of Student class
//
// Sources:        <Sources, if any>
//
// URL(s) of sources:
//                 <URLs of your sources, if any>
////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include "Student.hpp"

int Student::numStudents = 0;

Student::Student(std::string name, std::vector<double> &assignmentsScore, double projectScore)
	: name(name), assignmentsScore(assignmentsScore), projectScore(projectScore) {
		this->id = Student::numStudents;
		Student::numStudents++;
}

int Student::getId() {
	return id;
}

std::string Student::getName() {
	return name;
}

std::vector<double>& Student::getAssignmentsScore() {
	return assignmentsScore;
}

double Student::getProjectScore() {
	return projectScore;
}

int Student::getNumStudents() {
	return numStudents;
}

void Student::printDetails() {
	std::cout << "STUDENT DETAILS:" << std::endl;
	std::cout << "Id = " << id << std::endl;
	std::cout << "Name = " << name << std::endl;
	std::cout << "Assignments = [";
	for (auto i = assignmentsScore.begin(); i != assignmentsScore.end() - 1; ++i) {
		std::cout << *i << ", ";
	}
	auto end = assignmentsScore.end() - 1;
	std::cout << *end << "]" << std::endl;
	std::cout << "Project = " << projectScore << std::endl;
	std::cout << "Total = " << getTotal() << std::endl;
	std::cout << "Grade = " << getGrade() << std::endl;
}


