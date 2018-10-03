////////////////////////////////////////////////////////////////////////////////
// File Name:      GradStudent.cpp
//
// Author:         Zihan Wang
// CS email:       zwang883@wisc.edu
//
// Description:    an implementation of GradStudent class
//
// Sources:        <Sources, if any>
//
// URL(s) of sources:
//                 <URLs of your sources, if any>
////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include "GradStudent.hpp"

GradStudent::GradStudent(std::string name,
		std::vector<double>& assignmentsScore,
		double projectScore,
		std::string researchArea,
		std::string advisor) : 
	Student(name, assignmentsScore, projectScore), 
	researchArea(researchArea), 
	advisor(advisor) {
}

std::string GradStudent::getResearchArea() {
	return researchArea;
}

std::string GradStudent::getAdvisor() {
	return advisor;
}

void GradStudent::printDetails() {
	Student::printDetails();
	std::cout << "Type = Graduate Student" << std::endl;
	std::cout << "Research Area = " << researchArea << std::endl;
	std::cout << "Advisor = " << advisor << std::endl;
}

double GradStudent::getTotal() {
	double avg_assignments_score = 0l;
	double sum_assignments_score = 0l;
	for (auto i = getAssignmentsScore().begin(); i != getAssignmentsScore().end(); ++i) {
		sum_assignments_score += *i;
	}
	avg_assignments_score = sum_assignments_score/ ((double) getAssignmentsScore().size());
	return avg_assignments_score * 0.5 + Student::getProjectScore() * 0.5;
}

std::string GradStudent::getGrade() {
	std::string result = "";
	if (GradStudent::getTotal() >= 80) {
		result = "CR";
	}
	else {
		result = "N";
	}
	return result;
}


