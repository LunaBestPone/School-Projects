////////////////////////////////////////////////////////////////////////////////
// Main File:        generate_magic.c
// This File:        generate_magic.c
// Other Files:      (name of all other files if any)
// Semester:         CS 354 Fall 2017
//
// Author:           Zihan Wang
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

// Structure representing Square
// size: dimension(number of rows/columns) of the square
// array: 2D array of integers
typedef struct _Square {
	int size;
	int **array;
} Square;

int get_square_size();
Square * generate_magic(int size);
void write_to_file(Square * square, char *filename);

int main(int argc, char *argv[])
{
	// Check input arguments to get filename
	if (argc != 2) {
		printf("Usage: ./generate_magic <filename>\n");
		exit(1);
	}
	char* filename = *(argv + 1);
	// Get size from user
	int size = get_square_size();
	// Generate the magic square
	Square* matrix = generate_magic(size);
	// Write the square to the output file
	write_to_file(matrix, filename);
	// freeing the pointers
	for(int i = 0; i < size; ++i) {
		free(*(matrix->array + i));
	}
	free(matrix -> array);
	free(matrix);
	free(filename);
	return 0;
}

/* get_square_size prompts the user for the magic square size
 * checks if it is an odd number >= 3 and returns the number
 */
int get_square_size()
{
	printf("Enter size of magic square, must be odd\n");
	int size = 1;
	fscanf(stdin, "%i", &size);
	while (size < 3 || size % 2 == 0) {
		printf("Size must be an odd number >= 3.\n");
		fscanf(stdin, "%i", &size);
	}
	return size;
}

/* generate_magic constructs a magic square of size n
 * using the Siamese algorithm and returns the Square struct
 */
Square * generate_magic(int n)
{
	Square * newSquare = malloc(sizeof(Square));
	newSquare -> size = n;
	// the rows
	newSquare -> array = malloc(n * sizeof(int*));
	// the cols
	for(int i = 0; i < n; ++i) {
		*(newSquare -> array + i) = calloc(n, sizeof(int));
	}
	// the Siamese algo
	int currentRow = 0;	
	int currentCol = (n - 1)/2;
	for(int i = 1; i <= n*n; ++i) {
		// set the number there
		*(*(newSquare ->array + currentRow) + currentCol) = i;
		int nextRow = currentRow - 1;
		if(nextRow < 0) {
			nextRow = n - 1;
		}
		int nextCol = currentCol + 1;
		if(nextCol >= n) {
			nextCol = 0;
		}
		// if is empty at nextRow, nextCol, go there
		if(*(*(newSquare -> array + nextRow) + nextCol) == 0) {
			currentRow = nextRow;
			currentCol = nextCol;
		}
		// instead, shift down a box
		else {
			currentRow += 1;
			if(currentRow >= n) {
				currentRow = 0;
			}
		}
	}
		
	return newSquare;
}

/* write_to_file opens up a new file(or overwrites the existing file)
 * and writes out the square in the format expected by verify_hetero.c
 */
void write_to_file(Square * square, char *filename)
{
	// create a pointer to file fp
	FILE* fp = fopen(filename, "w");
	// printing the size
	fprintf(fp, "%i\n", square -> size);
	// printing the matrix
	for(int row = 0; row < square -> size; ++row){
		for (int col = 0; col < square -> size - 1; ++col) {
			fprintf(fp, "%i,", *(*(square -> array + row) + col));
		}
		fprintf(fp, "%i\n", *(*(square -> array + row) + square -> size - 1));
	}
	// close the file
	fclose(fp);
}
