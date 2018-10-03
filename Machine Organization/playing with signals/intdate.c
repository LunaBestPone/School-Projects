////////////////////////////////////////////////////////////////////////////////
// Main File:        intdate.c
// This File:        intdate.c
// Other Files:      (name of all other files if any)
// Semester:         CS 354 Fall 2017
//
// Author:           Zihan Wang
// Email:            zwang883
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
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

int counter = 0;
//handlers goes here
void handler_SIGALRM() {
	time_t curtime;
	time(&curtime);
	printf("PID: %d | Current Time: %s", getpid(), ctime(&curtime));
	alarm(3);
}

void handler_SIGUSR1() {
	counter++;
	printf("SIGUSR1 caught!\n");
}

void handler_SIGINT() {
	printf("\nSIGINT received.\nSIGUSR1 was received %d times. Exiting now.\n", counter);
	exit(0);
}

int main() {
	printf("Pid and time will be printed every 3 seconds.\nEnter ^C to end the program.\n");
	//declare sigactions
	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = handler_SIGALRM;
	sa.sa_flags = 0;

	struct sigaction sa1;
	memset(&sa1, 0, sizeof(sa1));
	sa1.sa_handler = handler_SIGUSR1;
	sa1.sa_flags = 0;

	struct sigaction sa2;
	memset(&sa2, 0, sizeof(sa2));
	sa2.sa_handler = handler_SIGINT;
	sa2.sa_flags = 0;

	if(sigaction(SIGALRM, &sa, NULL) != 0) {
		fprintf(stderr, "error REG. SIGALRM");
		exit(1);
	}

	if(sigaction(SIGUSR1, &sa1, NULL) != 0) {
		fprintf(stderr, "error REG. SIGUSR1");
		exit(1);
	}

	if(sigaction(SIGINT, &sa2, NULL) != 0) {
		fprintf(stderr, "error REG. SIGINT");
		exit(1);
	}
	alarm(3);
	while(1) {
	}
}
