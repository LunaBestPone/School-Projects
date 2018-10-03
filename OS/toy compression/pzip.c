#include <unistd.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sysinfo.h>

typedef struct __jobInfo {
	char* init;
	size_t wksz;
	int idx;
	int fidx;
} jobInfo;

int nf;
int nprocs;
int* fds;
size_t* fszs;
size_t* frszs;
char** maps;
jobInfo** jlist;
int jlistsz = 256;
int chunksz = 131072;
int numfull;
int fillptr;
int useptr;
char** output;
int jf;
int jc;
int outputsz;
int e;
size_t fsznz;


pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t jflock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t oplock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t frszslock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t elock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t fill = PTHREAD_COND_INITIALIZER;
pthread_cond_t empty = PTHREAD_COND_INITIALIZER;

void do_fill(char* initptr, size_t wksz, int indx, int fidx) {
	jobInfo* cur = malloc(sizeof(jobInfo));
	cur->init = initptr;
	cur->wksz = wksz;
	cur->idx = indx;
	cur->fidx = fidx;

	jlist[fillptr] = cur;
	fillptr = (fillptr + 1) % jlistsz;
	numfull++;
}

jobInfo* do_get() {
	jobInfo* temp = jlist[useptr];
	useptr = (useptr + 1) % jlistsz;
	numfull--;
	pthread_mutex_lock(&jflock);
	jf++;
	pthread_mutex_unlock(&jflock);

	return temp;
}

void* consumer(void* arg) {
	while(1) {
		pthread_mutex_lock(&lock);
		pthread_mutex_lock(&jflock);
		if(jf == jc) {
			pthread_mutex_unlock(&jflock);
			pthread_cond_signal(&fill);
			pthread_mutex_unlock(&lock);
			return NULL;
		}
		else {
			pthread_mutex_unlock(&jflock);
		}
		while(numfull == 0) {
			pthread_mutex_lock(&jflock);
			if(jf == jc) {	
				pthread_mutex_unlock(&jflock);
				pthread_cond_signal(&fill);
				pthread_mutex_unlock(&lock);
				return NULL;
			}
			else {
				pthread_mutex_unlock(&jflock);
			}
			pthread_cond_wait(&fill, &lock);
			pthread_mutex_lock(&elock);
			if (e == 1) {
				pthread_mutex_unlock(&elock);
				pthread_cond_signal(&fill);
				pthread_mutex_unlock(&lock);
				return NULL;
			}
			else {
				pthread_mutex_unlock(&elock);
			}
		}
		jobInfo* temp = do_get();
		pthread_cond_signal(&empty);
		pthread_mutex_unlock(&lock);

		char* init = temp->init;
		size_t wksz = temp->wksz;
		int idx = temp->idx;
		int fidx = temp->fidx;

		char* cur = init;
		int count = 0;
		char* cnkout = calloc(chunksz, 5);
		int cnkoutcnt = 0;
		for(int i = 0; i < wksz; i++) {
			if(*(init + i) == *cur) {
				count++;
			}
			else {
				int* intloc = (int*) (&cnkout[cnkoutcnt * 5]);
				*intloc = count;
				cnkout[cnkoutcnt*5 + 4] = *cur;
				cnkoutcnt++;
				cur = init + i;
				count = 1;
			}
		}
		int* intloc = (int*) (&cnkout[cnkoutcnt * 5]);
		*intloc = count;
		cnkout[cnkoutcnt*5 + 4] = *cur;
		pthread_mutex_lock(&frszslock);
		frszs[fidx] -= wksz;
		if(frszs[fidx] == 0) {
			pthread_mutex_unlock(&frszslock);
			munmap(maps[fidx], fszs[fidx]);
		}
		else {
			pthread_mutex_unlock(&frszslock);
		}
		pthread_mutex_lock(&oplock);
		while(idx > outputsz) {
			outputsz *= 2;
			output = realloc(output, sizeof(char*) * outputsz);
		}
		output[idx] = cnkout;
		pthread_mutex_unlock(&oplock);
		free(temp);
	}
	return NULL;
}


int main(int argc, char* argv[]) {
	if(argc == 1) {
		printf("pzip: file1 [file2 ...]\n");
		exit(1);
	}
	//var init
	nf = argc-1;
	nprocs = get_nprocs();
	fds = malloc(sizeof(int) * (argc - 1));
	frszs = malloc(sizeof(size_t) * (argc - 1));
	fszs = malloc(sizeof(size_t) * (argc - 1));
	maps = malloc(sizeof(char*) * (argc - 1));
	struct stat temp;
	for(int i = 1; i < argc; i++) {
		fds[i-1] = open(argv[i], O_RDONLY);
		fstat(fds[i-1], &temp);
		fszs[i-1] = temp.st_size;
		frszs[i-1] = temp.st_size;
		maps[i-1] = mmap(0,fszs[i-1],PROT_READ,MAP_SHARED,fds[i-1],0);
	}
	jlist = malloc(sizeof(jobInfo*) * jlistsz);
	numfull = 0;
	fillptr = 0;
	useptr = 0;
	jc = -1;
	jf = 0;
	fsznz = 0;
	for(int i = 0; i < nf; i++) {
		if (fszs[i] == 0) {
			continue;
		}
		else {
			fsznz = fszs[i];
		}
	}
	if (fsznz == 0) {
		fsznz = 1;
	}
	output = malloc(sizeof(char*) * fsznz);
	outputsz = fsznz;
	e = 0;

	//consumer creation
	pthread_t cid[nprocs];
	for(int i = 0; i < nprocs; i++) {
		pthread_create(&cid[i], NULL, consumer, NULL);
	}

	int done = 0;
	int fcnter = 0;
	int chnkcnter = 0;
	int idxcnter = 0;
	while(done != 1) {
		pthread_mutex_lock(&lock);
		while (numfull == jlistsz) {
			pthread_cond_wait(&empty,&lock);
		}
		if (fszs[fcnter] == 0) {
			munmap(maps[fcnter], fszs[fcnter]);
			fcnter++;
			pthread_mutex_unlock(&lock);
			continue;
		}
		if(fcnter < argc - 1) {
			if(chnkcnter * chunksz + chunksz <= fszs[fcnter] - 1) {
				do_fill(maps[fcnter] + chnkcnter * chunksz, chunksz, idxcnter, fcnter);
				chnkcnter++;
				idxcnter++;
			}
			else {
				do_fill(maps[fcnter] + chnkcnter * chunksz, (fszs[fcnter] - (chnkcnter * chunksz)), idxcnter, fcnter);
				fcnter++;
				chnkcnter = 0;
				idxcnter++;
			}
			pthread_cond_signal(&fill);
		}
		else {
			done = 1;
		}
		pthread_mutex_unlock(&lock);
	}
	jc = idxcnter;
	
	if (jc == 0) {
		jc = 0;
		pthread_mutex_lock(&elock);
		e = 1;
		pthread_mutex_unlock(&elock);
		pthread_cond_signal(&fill);
		for(int i = 0; i < nprocs; i++) {
			pthread_join(cid[i], NULL);
		}
		free(output);
		for(int i = 0; i < argc - 1; i++) {
			close(fds[i]);
		}
		free(fds);
		free(fszs);
		free(frszs);
		free(jlist);
		free(maps);
		exit(0);
	}	

	//wait for consumer
	for(int i = 0; i < nprocs; i++) {
		pthread_join(cid[i], NULL);
	}

	char* result = malloc(5 * chunksz);
	int resultsz = chunksz;
	int outamount = 0;
	char pre = output[0][4];
	int precount = 0;
	for(int i = 0; i < jc; i++) {
		for(int j = 0; j < chunksz; j++) {
			if (*((int*) (output[i] + j*5)) == 0) {
				break;
			}
			else {
				char cur = output[i][j*5+4];
				int curcount = *((int*) (output[i] + j*5));
				if(pre == cur) {
					precount = precount + curcount;
				}
				else {
					int* intloc = (int*) (&result[5*outamount]);
					*intloc = precount;
					result[outamount*5+4] = pre;
					pre = cur;
					precount = curcount;
					outamount++;
					if(outamount > resultsz) {
						resultsz += chunksz; 
						result = realloc(result, resultsz * 5);
					}
				}
			}
		}
	}
	int* intloc = (int*) (&result[5*outamount]);
	*intloc = precount;
	result[5*outamount + 4] = pre;
	outamount++;
	fwrite(result, 5, outamount, stdout);
	for(int i = 0; i < jc; i++) {
		free(output[i]);
	}
	free(output);
	free(result);
	for(int i = 0; i < argc - 1; i++) {
		close(fds[i]);
	}
	free(fds);
	free(fszs);
	free(frszs);
	free(jlist);
	free(maps);
	return 0;
}

	
	
