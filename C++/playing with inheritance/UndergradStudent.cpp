////////////////////////////////////////////////////////////////////////////////
// File Name:      UndergradStudent.cpp
//
// Author:         Zihan Wang
// CS email:       zwang883@wisc.edu
//
// Description:    an implementation of UndergradStudent class
//
// Sources:        <Sources, if any>
//
// URL(s) of sources:
//                 <URLs of your sources, if any>
////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include "UndergradStudent.hpp"

UndergradStudent::UndergradStudent(std::string name,
		std::vector<double> &assignmentsScore,
		double projectScore,
		std::string residenceHall,
		std::string yearInCollege) 
	: Student(name, assignmentsScore, projectScore), 
	residenceHall(residenceHall), 
	yearInCollege(yearInCollege) {
}

std::string UndergradStudent::getResidenceHall() {
	return residenceHall;
}

std::string UndergradStudent::getYearInCollege() {
	return yearInCollege;
}

void UndergradStudent::printDetails() {
	Student::printDetails();
	std::cout << "Type = Undergraduate Student" << std::endl;
	std::cout << "Residence Hall = " << residenceHall << std::endl;
	std::cout << "Year in College = " << yearInCollege << std::endl;
}

double UndergradStudent::getTotal() {
	double avg_assignments_score = 0l;
	double sum_assignments_score = 0l;
	for (auto i = getAssignmentsScore().begin(); i != getAssignmentsScore().end(); ++i) {
		sum_assignments_score += *i;
	}
	avg_assignments_score = sum_assignments_score/ ((double) getAssignmentsScore().size());
	return avg_assignments_score * 0.7 + Student::getProjectScore() * 0.3;
}

std::string UndergradStudent::getGrade() {
	std::string result = "";
	if (UndergradStudent::getTotal() >= 70) {
		result= "CR";
	}
	else {
		result = "N";
	}
	return result;
}

