////////////////////////////////////////////////////////////////////////////////
// Main File:        division.c
// This File:        division.c
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
#include <stdio.h>
#include <signal.h>
#include <string.h>

int counter = 0;
//handlers goes here
void handler_SIGFPE() {
	printf("Error: a division by 0 operation was attempted.\nTotal number of operations completed successfully: %d\nThe program will be terminated.\n", counter);
	exit(0);
}

void handler_SIGINT() {
	printf("\nTotal number of operations successfully completed: %d\nThe program will be terminated.\n", counter);
	exit(0);
}

int main() {
	//declare sigactions
	struct sigaction sa1;
	memset(&sa1, 0, sizeof(sa1));
	sa1.sa_handler = handler_SIGFPE;
	sa1.sa_flags = 0;

	struct sigaction sa2;
	memset(&sa2, 0, sizeof(sa2));
	sa2.sa_handler = handler_SIGINT;
	sa2.sa_flags = 0;

	if (sigaction(SIGFPE, &sa1, NULL) != 0) {
		fprintf(stderr, "error REG. SIGALRM");
		exit(1);
	}
	
	if (sigaction(SIGINT, &sa2, NULL) != 0) {
		fprintf(stderr, "error REG. SIGALRM");
		exit(1);
	}

	while(1) {
		char buffer[100];
		printf("Enter first integer: ");
		fgets(buffer, 100, stdin);
		int n1 = atoi(buffer);
		
		memset(&buffer, 0, 100);

		printf("Enter second integer: ");
		fgets(buffer, 100, stdin);
		int n2 = atoi(buffer);

		int q = n1/n2;
		int r = n1%n2;
		printf("%d / %d is %d with a remainder of %d\n", n1, n2, q, r);
		counter++;
	}
}
