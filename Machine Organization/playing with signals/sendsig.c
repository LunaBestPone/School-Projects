////////////////////////////////////////////////////////////////////////////////
// Main File:        sendsig.c
// This File:        sendsig.c
// Other Files:      (name of all other files if any)
// Semester:         CS 354 Fall 2017
//
// Author:           Zihan Wang
// Email:            zwang883@wisc.edu
// CS Login:         zian
//
/////////// IF PAIR PROGRAMMING IS ALLOWED, COMPLETE THIS SECTION //////////////
//
// Pair Partner:     (name of your pair programming partner)
// Email:            (email address of your programming partner)
// CS Login:         (partner's CS login name)
//
/////////////////////////// OTHER SOURCES OF HELP //////////////////////////////
//                   fully acknowledge and credit all sources of help,
//                   other than Instructors and TAs.
//
// Persons:          Identify persons by name, relationship to you, and email.
//                   Describe in detail the the ideas and help they provided.
//
// Online sources:   avoid web searches to solve your problems, but if you do
//                   search, be sure to include Web URLs and description of 
//                   of any information you find.
//////////////////////////// 80 columns wide ///////////////////////////////////
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>

int main(int argc, char** argv) {
	if (argc != 3) {
		printf("Usage: <signal type> <pid>\n");
		exit(0);
	}
	int pid = atoi(argv[2]);
	char i[] = "-i";
	char u[] = "-u";
	if (strcmp(argv[1], i) == 0) {
		kill(pid, 2);
	}
	else if (strcmp(argv[1], u) == 0) {
		kill(pid, 10);
	}
	exit(0);
}
