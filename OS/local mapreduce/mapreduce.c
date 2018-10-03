#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "mapreduce.h"

int* numkey;
int num_partitions = -1;
int num_shashsize = 5000;
int* filestaken;
int numfiles = 0;
pthread_mutex_t ftlock;
pthread_mutex_t hashArraylock;
Partitioner pt = NULL;
struct keynode*** hashArray = NULL;
Mapper lmap = NULL;
Reducer lreducer = NULL;
struct valnode** getteroffsets = NULL;
int* usedupflag = NULL;
int ptn = 0;
pthread_mutex_t ptnlock;
struct keynode*** linear;

struct valnode {
	char* val;
	struct valnode* next;
};

struct keynode {
	char* key;
	struct keynode* next;

	struct valnode* valhead;
	struct valnode* valtail;
};

unsigned long MR_DefaultHashPartition(char *key, int num_partitions) {
	unsigned long hash = 5381;
	int c;
	while ((c = *key++) != '\0')
		hash = hash * 33 + c;
	return hash % num_partitions;
}


unsigned long hashcode(char* key) {
	if(pt == NULL)
		return MR_DefaultHashPartition(key, num_partitions);
	else
		return pt(key, num_partitions);
}

unsigned long secondhash(char* key) {
	return MR_DefaultHashPartition(key, num_shashsize);
}

void insert(char* key, char* value, Partitioner partition) {

	struct valnode* item = (struct valnode*) calloc(1,sizeof(struct valnode));
	item->val = value;
	int fhashIndex = -1;
	int shashIndex = -1;
	fhashIndex = hashcode(key);
	shashIndex = secondhash(key);

	pthread_mutex_lock(&hashArraylock);
	if(hashArray[fhashIndex] == NULL) {
		struct keynode** keyarray = (struct keynode**) calloc(num_shashsize, sizeof(struct keynode*));
		hashArray[fhashIndex] = keyarray;
		struct keynode* tkeynode = (struct keynode*) calloc(1,sizeof(struct keynode));
		keyarray[shashIndex] = tkeynode;
		tkeynode->key = key;
		tkeynode->valtail = item;
		numkey[fhashIndex]++;
		pthread_mutex_unlock(&hashArraylock);
		tkeynode->valhead = item;
	}
	else {
		if(hashArray[fhashIndex][shashIndex] == NULL) {
			struct keynode* tkeynode = (struct keynode*) calloc(1,sizeof(struct keynode));
			hashArray[fhashIndex][shashIndex] = tkeynode;
			tkeynode->key = key;
			tkeynode->valtail = item;
			numkey[fhashIndex]++;
			pthread_mutex_unlock(&hashArraylock);
			tkeynode->valhead = item;
		}
		else {
			struct keynode* pre = NULL;
			struct keynode* cur = hashArray[fhashIndex][shashIndex];
			while(cur != NULL) {
				if(strcmp(cur->key,key) == 0) {
					cur->valtail->next = item;
					cur->valtail = item;
					pthread_mutex_unlock(&hashArraylock);
					free(key);
					return;
				}
				else {
					pre = cur;
					cur = cur->next;
				}
			}
			struct keynode* tkeynode = (struct keynode*) calloc(1,sizeof(struct keynode));
			pre->next = tkeynode;
			tkeynode->key = key;
			tkeynode->valtail = item;
			numkey[fhashIndex]++;
			pthread_mutex_unlock(&hashArraylock);
			tkeynode->valhead = item;
		}
	}
}

//need some global pointer
char* get_next(char* key, int partition_number) {
	if(getteroffsets[partition_number] == NULL) {
		usedupflag[partition_number]++;
		if(usedupflag[partition_number] == numkey[partition_number]) return NULL;
		struct keynode* curkey = linear[partition_number][usedupflag[partition_number]];
		getteroffsets[partition_number] = curkey->valhead;
		return NULL;
	}
	else {
		char* temp = getteroffsets[partition_number]->val;
		getteroffsets[partition_number] = getteroffsets[partition_number]->next;
		return temp;
	}
}

void MR_Emit(char* key, char* value) {
	char* keyd = strdup(key);
	char* valued = strdup(value);
	insert(keyd, valued, pt);
}

void* mapH(void* args) {
	char** argv = (char**) args;
	pthread_mutex_lock(&ftlock);
	for(int i = 0; i < numfiles; i++) {
		if (filestaken[i] == 0) {
			filestaken[i] = 1;
			pthread_mutex_unlock(&ftlock);
			lmap(argv[i+1]);
			pthread_mutex_lock(&ftlock);
		}
	}
	pthread_mutex_unlock(&ftlock);
	return NULL;
}

int cmpfunc(const void* a, const void* b) {
	return strcmp((*(struct keynode**)a)->key,(*(struct keynode**)b)->key);
}

void* reduceH(void* args) {
	pthread_mutex_lock(&ptnlock);
	int ptnumber = ptn;
	ptn++;
	pthread_mutex_unlock(&ptnlock);
	
	if(hashArray[ptnumber] == NULL) {
		return NULL;
	}
	else {
		int counter = 0;
		linear[ptnumber] = (struct keynode**) calloc(numkey[ptnumber], sizeof(struct keynode*));
		for(int i = 0; i < num_shashsize; i++) {
			if(hashArray[ptnumber][i] != NULL) {
				struct keynode* cur = hashArray[ptnumber][i];
				while(cur != NULL) {
					linear[ptnumber][counter++] = cur;
					cur = cur->next;
				}
			}
		}
		//qsort linear here
		qsort(linear[ptnumber], numkey[ptnumber], sizeof(struct keynode*), cmpfunc);
		getteroffsets[ptnumber] = linear[ptnumber][0]->valhead;
		for(int i = 0; i < numkey[ptnumber]; i++) {
			lreducer(linear[ptnumber][i]->key, (Getter) &get_next, ptnumber);
		}
		for(int i = 0; i < numkey[ptnumber]; i++) {
			struct valnode* cur = linear[ptnumber][i]->valhead;
			struct valnode* temp = NULL;
			while(cur != NULL) {
				temp = cur->next;
				free(cur->val);
				free(cur);
				cur = temp;
			}
			free(linear[ptnumber][i]->key);
			free(linear[ptnumber][i]);
		}
		free(linear[ptnumber]);
		free(hashArray[ptnumber]);
		return NULL;
	}
}
	
void MR_Run(int argc, char* argv[], Mapper map, int num_mappers, Reducer reduce, int num_reducers, Partitioner partition) {
	num_partitions = num_reducers;
	pthread_t mthreads[num_mappers];
	pthread_t rthreads[num_reducers];
	numfiles = argc - 1;
	filestaken = (int*) calloc(numfiles, sizeof(int));
	pt = partition;
	lmap = map;
	lreducer = reduce;
	hashArray = (struct keynode***) calloc(num_partitions, sizeof(struct keynode**));
	getteroffsets = (struct valnode**) calloc(num_partitions, sizeof(struct valnode*));
	usedupflag = (int*) calloc(num_partitions, sizeof(int));
	linear = (struct keynode***) calloc(num_partitions, sizeof(struct keynode**));
	numkey = (int*) calloc(num_partitions, sizeof(int));

	for(int i = 0; i < num_mappers; i++) {
		pthread_create(&mthreads[i], NULL, mapH, (void*) argv);
	}
	for(int i = 0; i < num_mappers; i++) {
		pthread_join(mthreads[i], NULL);
	}
	for(int i = 0; i < num_reducers; i++) {
		pthread_create(&rthreads[i],NULL, reduceH,(void*) &i);
	}
	for(int i = 0; i < num_reducers; i++) {
		pthread_join(rthreads[i], NULL);
	}
	free(hashArray);
	free(getteroffsets);
	free(usedupflag);
	free(filestaken);
	free(linear);
	free(numkey);
	ptn = 0;
}
