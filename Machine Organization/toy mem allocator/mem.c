////////////////////////////////////////////////////////////////////////////////
// Main File:        mem.c
// This File:        mem.c
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
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include "mem.h"
#include "limits.h"

/*
 * This structure serves as the header for each allocated and free block
 * It also serves as the footer for each free block
 * The blocks are ordered in the increasing order of addresses 
 */
typedef struct blk_hdr {   		

	int size_status;
  
	/*
	* Size of the block is always a multiple of 8
	* => last two bits are always zero - can be used to store other information
	*
	* LSB -> Least Significant Bit (Last Bit)
	* SLB -> Second Last Bit 
	* LSB = 0 => free block
	* LSB = 1 => allocated/busy block
	* SLB = 0 => previous block is free
	* SLB = 1 => previous block is allocated/busy
	* 
	* When used as the footer the last two bits should be zero
	*/

	/*
	* Examples:
	* 
	* For a busy block with a payload of 20 bytes (i.e. 20 bytes data + an additional 4 bytes for header)
	* Header:
	* If the previous block is allocated, size_status should be set to 27
	* If the previous block is free, size_status should be set to 25
	* 
	* For a free block of size 24 bytes (including 4 bytes for header + 4 bytes for footer)
	* Header:
	* If the previous block is allocated, size_status should be set to 26
	* If the previous block is free, size_status should be set to 24
	* Footer:
	* size_status should be 24
	* 
	*/

} blk_hdr;

/* Global variable - This will always point to the first block
 * i.e. the block with the lowest address */
blk_hdr *first_block = NULL;

// Global variable - Total available memory 
int total_mem_size = 0;

/* 
 * Function for allocating 'size' bytes
 * Returns address of allocated block on success 
 * Returns NULL on failure 
 * Here is what this function should accomplish 
 * - Check for sanity of size - Return NULL when appropriate 
 * - Round up size to a multiple of 8 
 * - Traverse the list of blocks and allocate the best free block which can accommodate the requested size 
 * - Also, when allocating a block - split it into two blocks
 * Tips: Be careful with pointer arithmetic 
 */
void* Mem_Alloc(int size) {   	   
	// if requested size is larger than total memory in the heap, return NULL pointer
	if (size > total_mem_size) {
		return NULL;
	}
	// calculate the rounded size that include 4 byte header for 8 byte alignment
	int requiredSize = size + sizeof(blk_hdr*);
	int roundedSize = -1;
	if (requiredSize % 8 == 0) {
		roundedSize = requiredSize;
	}
	else {
		int pad = 8 - requiredSize % 8;
		roundedSize = requiredSize + pad;
	}
	// if after rounding, the size exceeds heap capacity, return NULL pointer
	if (roundedSize > total_mem_size) {
		return NULL;
	}

	// prepare the linear search
	// set the iterator to the first block
	blk_hdr* it;
	it = first_block;
	// the pointer used to store optimal block's location
	blk_hdr* opt;
	// NULL to avoid warnings
	opt = NULL;
	// a bit used to keep track of whether current block pointed by iterator is busy or not
	int isBusy = -1;
	// a integer use to keep track of the difference between the size of a large enough block and the required size
	int difference = INT_MAX;
	while (it < (blk_hdr*) ((char*) first_block + total_mem_size)) {
		int t_size = it->size_status;
		// check the status and get the real size of the block
		if (it->size_status & 1) {
			isBusy = 1;
			t_size -=1;
		}
		else {
			isBusy = 0;
		}
		if (it->size_status & 2) {
			t_size -=2;
		}
		// if the block is free, and the size is larger than rounded required size including
		// the header, record the difference in size, if the difference is optimal, record
		// the difference, and store the location to opt pointer
		if (isBusy == 0 && t_size >= roundedSize) {
			if (difference > (t_size - roundedSize)) {
				difference = t_size - roundedSize;
				opt = it;
			}
		}
		// check if the difference right now is 0, if it is, modify the header of opt
		// and modify the p bit in the next block's header
		if (difference == 0) {
			// +1 because a bit should be 1, p bit unchanged for opt's header
			opt->size_status = t_size + 1 ;
			// locate the next block's header with it pointer
			it = (blk_hdr*) ((char*) it + t_size);
			// check if it is in our range
			if (it < (blk_hdr*) ((char*) first_block + total_mem_size)) {
				// update the p bit
				// I dont think it is neccessary to check footer here
				it->size_status += 2;
			}
			void* result;
			result = (blk_hdr*) ((char*) opt + sizeof(blk_hdr*));
			return result;
		}
		// if it block is not exact mach, increment it's location and check the status of
		// next it in the next iternation
		else {
			it = (blk_hdr*) ((char*) it + t_size);
		}
		// a iteration ends here
	}
	// after all the iterations are finished and we are yet in the function, it means no exact mach
	// we have two cases.
	// when difference is still INT_MAX, we did not update difference and opt,
	// that meansthere is no free block that satisfies our need in size.
	// when difference is not INT_MAX, we updated opt, but it needs to be splited
	// case 1:
	if (difference == INT_MAX) {
		return NULL;
	}
	// case 2:
	else {
		int t_size = opt->size_status;
		// get the acutal size of opt
		if (opt->size_status & 2) {
			t_size -= 2;
		}
		// create header for new free block, set location
		blk_hdr* newFree;
		newFree = (blk_hdr*) ((char*) opt + t_size - difference);
		// also set the status
		newFree->size_status = difference;
		// since previous block is definitely busy, and itself is free
		newFree->size_status += 2;
		// create the footer for the new free block, set location
		blk_hdr* newFreeFooter;
		newFreeFooter = (blk_hdr*) ((char*) newFree + difference - sizeof(blk_hdr*));
		newFreeFooter->size_status = newFree->size_status;
		// since we dont need p and a bit in footer
		newFreeFooter->size_status -= 2;

		// now we update the header of opt
		opt->size_status -= difference;
		// availability becomes 1
		opt->size_status += 1;
		// ok to return opt now
		void* result;
		result = (blk_hdr*) ((char*) opt + sizeof(blk_hdr*));
		return result;
	}
}

/* 
 * Function for freeing up a previously allocated block 
 * Argument - ptr: Address of the block to be freed up 
 * Returns 0 on success 
 * Returns -1 on failure 
 * Here is what this function should accomplish 
 * - Return -1 if ptr is NULL or not within the range of memory allocated by Mem_Init()
 * - Return -1 if ptr is not 8 byte aligned or if the block is already freed
 * - Mark the block as free 
 * - Coalesce if one or both of the immediate neighbours are free 
 */
int Mem_Free(void *ptr) {   	   
	// null pointer, return -1;
	if (ptr == NULL) {
		return -1;
	}
	// set ptr to the header instead of payload, for the convinence of operations
	ptr = (void*) ((char*) ptr - sizeof(blk_hdr*));
	// out of range, return -1;
	if (first_block > (blk_hdr*) ptr || (blk_hdr*) ptr >= (blk_hdr*) ((char*) first_block + total_mem_size)) {
		return -1;
	}
	int t_size = ((blk_hdr*) ptr)->size_status;
	// if it is already freed, return -1;
	if (!(t_size & 1)) {
		return -1;
	}
	// if not, calculate the real size
	else {
		t_size -= 1;
	}
	// get the size if previous block is busy
	if (t_size & 2) {
	       t_size -= 2;
	}
	// if the real size is not a multiple of 8, return -1
	if (t_size%8 != 0) {
		return -1;
	}
	// check forward (the locations after ptr)
	// the free size after ptr
	int fw_total_size = 0;
	// keep track of the real size of the block pointed by the foward iterator
	int fwit_t_size = 0;
	// set the foward iterator and initiate the linear search
	blk_hdr* fwit = (blk_hdr*) ((char*) ((blk_hdr*) ptr) + t_size);
	while (fwit < (blk_hdr*) ((char*)first_block + total_mem_size)) {
		fwit_t_size = fwit->size_status;
		// if previous busy, calculate real size
		if (fwit->size_status & 2) {
			fwit_t_size -= 2;
		}
		// if fwit block is free, onward
		if (!(fwit->size_status & 1)) {
			fwit = (blk_hdr*) ((char*) fwit + fwit_t_size);
			fw_total_size += fwit_t_size;
			continue;
		}
		// if fwit block is busy, stop here
		else {
			break;
		}
	}
	// check backward
	// same logic, but slightly different from check foward
	int bck_total_size = 0;
	blk_hdr* bckit = (blk_hdr*) ptr;
	blk_hdr* pfooter;
	blk_hdr* pheader;
	while (bckit >= first_block) {
		// check if previous is free
		// if previous is free, first get to the footer, then header should be next bckit
		if (!(bckit->size_status & 2)) {
			pfooter = (blk_hdr*) ((char*) bckit - sizeof(blk_hdr*));
			pheader = (blk_hdr*) ((char*) bckit - pfooter->size_status);
			bck_total_size += pfooter->size_status;
			bckit = pheader;
		}
		// if previous is busy, stop here
		else {
			break;
		}
	}
	// the size of the new block is bck total + ptr size + fwd total
	// the new header location is at bckit
	// we modify bckit. since the bckit previous is definitely 
	// busy, p bit is 1, a bit is 0
	bckit->size_status = bck_total_size + t_size + fw_total_size + 2;
	// also set the footer
	blk_hdr* newFreeFooter = (blk_hdr*) ((char*) bckit + (bckit->size_status - 2) - sizeof(blk_hdr*));
	newFreeFooter->size_status = bckit->size_status - 2;
	// also set the header of next block
	blk_hdr* nextBlock = (blk_hdr*) ((char*) newFreeFooter + sizeof(blk_hdr*));
	if(nextBlock < (blk_hdr*) ((char*) first_block + total_mem_size)) {
		if(nextBlock->size_status & 2) {
			nextBlock->size_status -= 2;
		}
	}
	// return 0 here
	return 0;
}

/*
 * For testing purpose
 * To verify whether a block is double word aligned
 */
void *start_pointer;

/*
 * Function used to initialize the memory allocator
 * Not intended to be called more than once by a program
 * Argument - sizeOfRegion: Specifies the size of the chunk which needs to be allocated
 * Returns 0 on success and -1 on failure 
 */
int Mem_Init(int sizeOfRegion) {   	   
	int pagesize;
	int padsize;
	int fd;
	int alloc_size;
	void* space_ptr;
	static int allocated_once = 0;
  
if(0 != allocated_once) {
		fprintf(stderr,"Error:mem.c: Mem_Init has allocated space during a previous call\n");
		return -1;
	}
	if(sizeOfRegion <= 0) {
		fprintf(stderr,"Error:mem.c: Requested block size is not positive\n");
		return -1;
	}

	// Get the pagesize
	pagesize = getpagesize();

	// Calculate padsize as the padding required to round up sizeOfRegion to a multiple of pagesize
	padsize = sizeOfRegion % pagesize;
	padsize = (pagesize - padsize) % pagesize;

	alloc_size = sizeOfRegion + padsize;

	// Using mmap to allocate memory
	fd = open("/dev/zero", O_RDWR);
	if(-1 == fd){
		fprintf(stderr,"Error:mem.c: Cannot open /dev/zero\n");
		return -1;
	}
	space_ptr = mmap(NULL, alloc_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
	if (MAP_FAILED == space_ptr) {
		fprintf(stderr,"Error:mem.c: mmap cannot allocate space\n");
		allocated_once = 0;
		return -1;
	}
  
	allocated_once = 1;
 
	// for double word alignement
	alloc_size -= 4;
 
	// Intialising total available memory size
	total_mem_size = alloc_size;

	// To begin with there is only one big free block
	// initialize heap so that first block meets double word alignement requirement
	first_block = (blk_hdr*) space_ptr + 1;
	start_pointer = space_ptr;
  
	// Setting up the header
	first_block->size_status = alloc_size;

	// Marking the previous block as busy
	first_block->size_status += 2;

	// Setting up the footer
	blk_hdr *footer = (blk_hdr*) ((char*)first_block + alloc_size - 4);
	footer->size_status = alloc_size;
  
	return 0;
}

/* 
 * Function to be used for debugging 
 * Prints out a list of all the blocks along with the following information for each block 
 * No.	  : serial number of the block 
 * Status   : free/busy 
 * Prev	 : status of previous block free/busy
 * t_Begin  : address of the first byte in the block (this is where the header starts) 
 * t_End	: address of the last byte in the block 
 * t_Size   : size of the block (as stored in the block header)(including the header/footer)
 */ 
void Mem_Dump() {   	   
	int counter;
	char status[5];
	char p_status[5];
	char *t_begin = NULL;
	char *t_end = NULL;
	int t_size;

	blk_hdr *current = first_block;
	counter = 1;

	int busy_size = 0;
	int free_size = 0;
	int is_busy = -1;

	fprintf(stdout,"************************************Block list***********************************\n");
	fprintf(stdout,"No.\tStatus\tPrev\tt_Begin\t\tt_End\t\tt_Size\n");
	fprintf(stdout,"---------------------------------------------------------------------------------\n");
  
	while (current < (blk_hdr*) ((char*)first_block + total_mem_size)) {
		t_begin = (char*)current;
		t_size = current->size_status;
	
		if (t_size & 1) {
			// LSB = 1 => busy block
			strcpy(status,"Busy");
			is_busy = 1;
			t_size = t_size - 1;
		}
		else {
			strcpy(status,"Free");
			is_busy = 0;
		}

		if (t_size & 2) {
			strcpy(p_status,"Busy");
			t_size = t_size - 2;
		}
		else 
			strcpy(p_status,"Free");

		if (is_busy) 
			busy_size += t_size;
		else 
			free_size += t_size;

		t_end = t_begin + t_size - 1;
	
		fprintf(stdout,"%d\t%s\t%s\t0x%08lx\t0x%08lx\t%d\n", counter, status, p_status, 
					(unsigned long int)t_begin, (unsigned long int)t_end, t_size);
	
		current = (blk_hdr*)((char*)current + t_size);
		counter = counter + 1;
	}

	fprintf(stdout,"---------------------------------------------------------------------------------\n");
	fprintf(stdout,"*********************************************************************************\n");
	fprintf(stdout,"Total busy size = %d\n", busy_size);
	fprintf(stdout,"Total free size = %d\n", free_size);
	fprintf(stdout,"Total size = %d\n", busy_size + free_size);
	fprintf(stdout,"*********************************************************************************\n");
	fflush(stdout);

	return;
}
