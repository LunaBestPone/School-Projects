////////////////////////////////////////////////////////////////////////////////
// Main File:        verify_hetero.c
// This File:        verify_hetero.c
// Other Files:      (name of all other files if any)
// Semester:         CS 354 Fall 2017
//
// Author:           Zihan
// Email:            zwang883@wisc.edu
// CS Login:         zian
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
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

// Structure representing Square
// size: dimension(number of rows/columns) of the square
// array: 2D array of integers
typedef struct _Square {
	int size;
	int **array;
} Square;

Square * construct_square(char *filename);
void insertion_sort(int* arr, int size);
int verify_hetero(Square * square);

int main(int argc, char *argv[])
{
	// Check input arguments to get filename
	if(argc != 2) {
		printf("Usage: ./verify_hetero <filename>\n");
		exit(1);
	}
	// Construct square
	Square* matrix = construct_square(*(argv + 1));
	// Verify if it's a heterosquare and print true or false
	if(verify_hetero(matrix)) {
		printf("true\n");
	}
	else{
		printf("false\n");
	}
	for(int i = 0; i < matrix -> size; i++) {
		free(*(matrix -> array + i));
	}
	free(matrix -> array);
	free(matrix);
	return 0;
}

/* construct_square reads the input file to initialize a square struct
 * from the contents of the file and returns the square.
 * The format of the file is defined in the assignment specifications
 */
Square * construct_square(char *filename)
{

	// Open and read the file
	FILE *fp = fopen(filename, "r");
	if (fp == NULL) {
		printf("Cannot open file for reading.\n");
		exit(1);
	}
	// Read the first line to get the square size
	int squareSize;
	fscanf(fp, "%i", &squareSize);
	// Initialize a new Square struct of that size
	Square* newSquare = malloc(sizeof(Square));
	newSquare -> size = squareSize;
	// initialize the internal 2d array
	newSquare -> array = malloc(squareSize * sizeof(int*));
	for(int i = 0; i < squareSize; ++i) {
		*(newSquare -> array + i) = malloc(squareSize * sizeof(int));
	}
	// Read the rest of the file to fill up the square
	for(int row = 0; row < squareSize; ++row) {
		for(int col = 0; col < squareSize - 1; ++col) {
			fscanf(fp, "%i,", *(newSquare -> array + row) + col);
		}
		fscanf(fp,"%i", *(newSquare -> array + row) + squareSize - 1);
	}
	// done with the file, closing
	fclose(fp);
	return newSquare;
}

/* insertion_sort sorts the arr in ascending order
 *
 */
void insertion_sort(int* arr, int size)
{
        // Sort the arr
	for(int c = 0; c < size; ++c) {
		int min = INT_MAX;
		for(int d = c; d < size; ++d) {
			if(*(arr + d) < min) {
				min = *(arr + d);
				int temp = *(arr + d);
				*(arr + d) = *(arr + c);
				*(arr + c) = temp;
			}
		}
	}
}

/* verify_hetero verifies if the square is a heterosquare
 * 
 * returns 1(true) or 0(false)
 */
int verify_hetero(Square * square)
{
	// since we have vertical, horizontal, and 2 main diagnol sum, there should be a
	// 2 * size + 2 array to hold all the sums

	int* sumContainer = calloc((2 * (square -> size) + 2), sizeof(int));
	for(int i = 0; i < (square -> size); ++i) {
		for(int j = 0; j < (square -> size); ++j) {
			// sums for rows
			*(sumContainer + i) += *(*(square -> array + i) + j);
		}
	}
	for(int i = 0; i < (square -> size); ++i) {
		for(int j = 0; j < (square -> size); ++j) {
			// sums for cols
			*(sumContainer + (square -> size) + i) += *(*(square -> array + j) + i);
		}
	}
	for(int i = 0; i < (square -> size); ++i) {
		// sum for top left to buttom right diagnol
		*(sumContainer + 2 * (square -> size)) += *(*(square -> array + i ) + i);
	}
	for(int i = ((square -> size) - 1); i >=0; --i) {
		for(int j = 0; j < (square -> size); ++j) {
			// sum for top right to buttom left diagnol
			*(sumContainer + 2 * (square -> size) + 1) += *(*(square -> array + j) + i);
		}
	}
	// the building of sumContainer is complete by this line
	// initiate the insertion sort to sumContainer
	insertion_sort(sumContainer, 2 * (square -> size) + 2);
	// check if the array contains duplicate integers
	for(int i = 1; i < 2 * (square -> size) + 2; ++i) {
		if(*(sumContainer + i) - *(sumContainer + i - 1) == 0) {
			free(sumContainer);
			return 0;
		}
	}
	free(sumContainer);
	return 1;
}
