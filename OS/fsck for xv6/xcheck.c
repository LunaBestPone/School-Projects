#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
int fd;
void* map;
char* img;
size_t len;
struct dinode** iarray;
char** bitmap;
char** bitmapg;
		
struct dinode {
	short type;
	short major;
	short minor;
	short nlink;
	uint size;
	uint addrs[13];
};

struct superblock {
	uint size;
	uint nblocks;
	uint ninodes;
};

struct superblock* sp; 

void E(int i) {
	if(iarray != NULL) {
		free(iarray);
	}
	if(bitmap != NULL) {
		free(bitmap);
	}
	if(bitmapg != NULL) {
		for(int i = 0; i < sp->size/8; i++) {
			free(bitmapg[i]);
		}
	}
	free(bitmapg);
	munmap(img,len);
	close(fd);
	exit(i);
}

struct dirent {
	ushort inum;
	char name[14];
};

int main(int argc, char* argv[]) {

	int ninodes;
	int nblocks;



	if(argc != 2)  {
		fprintf(stderr, "Usage: xcheck <file_system_image>\n");
		E(1);
	}
	
	fd = open(argv[1], O_RDONLY);
	if(fd == -1) {
		fprintf(stderr, "image not found.\n");
		E(1);
	}
	len = lseek(fd, 0, SEEK_END);
	map = mmap(0, len, PROT_READ, MAP_SHARED, fd, 0);
	img = (char*) map;

	sp = (struct superblock*) (img + 0x200);
	ninodes = sp->ninodes;
	nblocks = sp->nblocks;
	//printf("size: %d\n", sp->size);
	//printf("nblocks: %d\n", sp->nblocks);

	int direct[sp->size];
	int indirect[sp->size];
	int inuseref[ninodes];
	int dirref[ninodes];

	for(int i = 0; i < sp->size; i++) {
		direct[i] = 0;
		indirect[i] = 0;
	}

	for(int i= 0; i < ninodes; i++) {
		inuseref[i] = 0;
		dirref[i] = 0;
	}

	iarray = calloc(ninodes, sizeof(struct dinode*));
	for(int i = 0; i < ninodes; i++) {
		iarray[i] = (struct dinode*) (img + 0x400 + 0x40 * i);
		//printf("i: %d\n", i);
		//printf("inodetype: %d\n", iarray[i]->type);
	}

	bitmap = calloc(sp->size/8, sizeof(char*));
	for(int i = 0; i < sp->size/8; i++) {
		bitmap[i] = (char*) (img + (sp->size - nblocks - 1)*512 + 0x1 * i);
	}
	bitmapg = calloc(sp->size/8,sizeof(char*));
	for(int j = 0; j < sp->size/8; j++) {
		bitmapg[j] = calloc(1, sizeof(char));
	}
	for(int m = 0; m < (sp->size - nblocks)/8; m++) {
		for(int n = 0; n < 8; n++) {
			*bitmapg[m] = ((1<<n) | *bitmapg[m]);
		}
	}
	//printf("integer: %d\n", (sp->size - nblocks)/8);
	//printf("integer: %d\n", (sp->size - nblocks)%8);
	for(int m = 0; m < (sp->size - nblocks)%8; m++) {
		*bitmapg[(sp->size-nblocks)/8] = ((1<<m) | *bitmapg[3]);
	}

	for(int i = 0; i < ninodes; i++) {
		//1
		if (iarray[i]->type >> 2 != 0) {
			fprintf(stderr, "ERROR: bad inode.\n");
			E(1);
		}
		//2
		if (iarray[i]->type != 0) {
			for(int j = 0; j < 12; j++) {
				if((iarray[i]->addrs[j] != 0) && (iarray[i]->addrs[j] < sp->size - nblocks || iarray[i]->addrs[j] >= sp->size)) {
					//printf("%x\n", iarray[i]->addrs[j]);
					fprintf(stderr, "ERROR: bad direct address in inode.\n");
					E(1);
				}
			}
			if((iarray[i]->addrs[12] != 0) && (iarray[i]->addrs[12] < sp->size - nblocks || iarray[i]->addrs[12] >= sp->size)) {
				//printf("%x\n", iarray[i]->addrs[12]);
				fprintf(stderr, "ERROR: bad indirect address in inode.\n");
				E(1);
			}
			if(iarray[i]->addrs[12] != 0) {
				for(int j = 0; j < 128; j++) {
					uint* temp = (uint*)(img + (iarray[i]->addrs[12])*512 + sizeof(uint)*j);
					if(*temp != 0 && (*temp < sp->size - nblocks || *temp >= sp->size)) {
						//printf("inode type: %d\n", iarray[i]->type);
						//printf("indirect addrs: %d\n", iarray[i]->addrs[12]);
						//printf("%d\n", i);
						//printf("%d\n", *temp);
						fprintf(stderr, "ERROR: bad indirect address in inode.\n");
						E(1);
					}
				}
			}
		}
		//3
		if (i == 1) {
			if(iarray[i]->type == 1) {
				struct dirent* temp = (struct dirent*) (img + (iarray[i]->addrs[0])*512 + sizeof(struct dirent));
				if(temp->inum != i) {
					//printf("1\n");
					//printf("%d\n", temp->inum);
					fprintf(stderr, "ERROR: root directory does not exist.\n");
					E(1);
				}
			}
			else {
				//printf("2\n");
				fprintf(stderr, "ERROR: root directory does not exist.\n");
				E(1);
			}
		}
		//4
		/*
		if(i >= 0 && iarray[i]->type == 1) {
			printf("directory inode: %d\n", i);
		}
		*/
		if(i >= 1 && iarray[i]->type == 1) {
			struct dirent* self = (struct dirent*) (img + (iarray[i]->addrs[0])*512);
			struct dirent* parent = (struct dirent*) (img + (iarray[i]->addrs[0])*512 + sizeof(struct dirent));
			if(strcmp((self->name),".") != 0) {
				//printf("here\n");
				//printf("%s\n", self->name);
				fprintf(stderr, "ERROR: directory not properly formatted.\n");
				E(1);
			}
			if(strcmp((parent->name), "..") != 0) {
				//printf("%s\n", parent->name);
				fprintf(stderr, "ERROR: directory not properly formatted.\n");
				E(1);
			}
			if(self->inum != i) {
				//printf("%d\n", self->inum);
				fprintf(stderr, "ERROR: directory not properly formatted.\n");
				E(1);
			}
			for(int x = 0; x < 12; x++) {
				struct dirent** container = calloc(512/sizeof(struct dirent), sizeof(struct dirent*));
				for(int j = 0; j < (512/sizeof(struct dirent)); j++) {
					container[j] = (struct dirent*) (img + (iarray[i]->addrs[x])*512 + sizeof(struct dirent)*j);
					//printf("directory %d ref inode %d\n", i, container[j]->inum);
					
				}
				for(int j = 0; j < (512/sizeof(struct dirent)); j++) {
					inuseref[container[j]->inum]++;
					if(x == 0) {
						if(j > 1) {
							if (iarray[container[j]->inum]->type == 1) {
								dirref[container[j]->inum]++;
							}
						}
					}
					else {
						if(iarray[container[j]->inum]->type == 1) {
							dirref[container[j]->inum]++;
						}
					}
				}
				free(container);
			}
			if(iarray[i]->addrs[12] != 0) {
				for(int x = 0; x <512/sizeof(uint); x++) {
					uint* temp = (uint*) (img + iarray[i]->addrs[12]*512 + sizeof(uint)*x);
					if(*temp != 0) {
						struct dirent** container = calloc(512/sizeof(struct dirent), sizeof(struct dirent*));
						for(int j = 0; j < (512/sizeof(struct dirent)); j++) {
							container[j] = (struct dirent*) (img + *temp*512 + sizeof(struct dirent)*j);
							//printf("directory %d ref inode %d\n", i, container[j]->inum);
						}
						for(int j = 0; j < (512/sizeof(struct dirent)); j++) {
							inuseref[container[j]->inum]++;
							if(iarray[container[j]->inum]->type == 1) {
								dirref[container[j]->inum]++;
							}
						}
						free(container);
					}
				}
			}
		}
		//5
		if(iarray[i]->type != 0) {
			for(int j = 0; j < 13; j++) {
				if (iarray[i]->addrs[j] != 0) {
					if(j < 12) {
						direct[iarray[i]->addrs[j]]++;
					}
					/*
					else {
						indirect[iarray[i]->addrs[j]]++;
					}
					*/
					int bindex = (iarray[i]->addrs[j])/8;
					int boff = (iarray[i]->addrs[j])%8;
					*bitmapg[bindex] = ((1<<boff) | *bitmapg[bindex]);
					if(!(((1<<boff) & *bitmap[bindex]))) {
						/*
						printf("bindex is: %d\n", bindex);
						printf("boff is: %d\n", boff);
						printf("hex at bindex: %x\n", *bitmap[bindex]);
						printf("i: %d\n", i);
						printf("j: %d\n", j);
						printf("addrs: %x\n", iarray[i]->addrs[j]);
						*/
						fprintf(stderr, "ERROR: address used by inode but marked free in bitmap.\n");
						E(1);
					}
				}
			}
			if (iarray[i]->addrs[12] != 0) {
				for(int j = 0; j < 128; j++) {
					uint* temp = (uint*)(img + (iarray[i]->addrs[12])*512 + sizeof(uint)*j);
					if(*temp != 0) {
						indirect[*temp]++;
						int bindex = *temp/8;
						int boff = *temp%8;
						*bitmapg[bindex] = ((1<<boff) | *bitmapg[bindex]);
						if(!(((1<<boff) & *bitmap[bindex]))) {
							/*
							printf("bindex is: %d\n", bindex);
							printf("boff is: %d\n", boff);
							printf("hex at bindex: %x\n", *bitmap[bindex]);
							printf("i: %d\n", i);
							printf("j: %d\n", j);
							printf("addrs: %x\n", iarray[i]->addrs[j]);
							*/
							fprintf(stderr, "ERROR: address used by inode but marked free in bitmap.\n");
							E(1);
						}
					}
				}
			}
		}
	}
	/*
	for(int i = 0; i <sp->size/8;i++) {
		printf("i : %d\n", i);
		printf("g : %x\n", *bitmapg[i]);
		printf("o : %x\n\n", *bitmap[i]);
	}
	*/
	//6
	for(int i = 0; i < sp->size/8; i++) {
		if(*bitmapg[i] != *bitmap[i]) {
			//printf("i is: %d\n", i);
			//printf("g : %x\n", *bitmapg[i]);
			//printf("original : %x\n", *bitmap[i]);
			fprintf(stderr, "ERROR: bitmap marks block in use but it is not in use.\n");
			E(1);
		}
	}
	//7
	for(int i = 0; i < sp->size; i++) {
		if(direct[i] > 1) {
			fprintf(stderr, "ERROR: direct address used more than once.\n");
			E(1);
		}
	}
	//8
	for(int i = 0; i < sp->size; i++) {
		//printf("indirect[%d] == %d\n", i, indirect[i]);
		if(indirect[i] > 1) {
			fprintf(stderr, "ERROR: indirect address used more than once.\n");
			E(1);
		}
	}
	//9 10 11 12
	/*
	for(int i = 0; i < ninodes; i++) {
		printf("i %d inuseref %d\n", i, inuseref[i]);
	}
	*/
	for(int i = 1; i < ninodes; i++) {
		if(iarray[i]->type == 2) {
			if(inuseref[i]<=0) {
				//printf("i: %d\n", i);
				fprintf(stderr, "ERROR: inode marked use but not found in a directory.\n");
				E(1);
			}
			if(inuseref[i] != iarray[i]->nlink) {
				fprintf(stderr, "ERROR: bad reference count for file.\n");
				E(1);
			}
		}
		if(inuseref[i] > 0) {
			if(iarray[i]->type == 0) {
				fprintf(stderr, "ERROR: inode referred to in directory but marked free.\n");
				E(1);
			}
		}
		if(iarray[i]->type == 1) {
			if(i != 1) {
				if(dirref[i] > 1) {
					//printf("i is: %d\n", i);
					//printf("dirref: %d\n", dirref[i]);
					fprintf(stderr, "ERROR: directory appears more than once in file system.\n");
					E(1);
				}
			}
		}
	}
	E(0);
}
