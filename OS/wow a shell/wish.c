#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

char** path;

void ehandler() {
	char e[30] = "An error has occurred\n";
	write(STDERR_FILENO, e, strlen(e));
}

void loopOnce(char** userLine, size_t* n, FILE* inputStream, int paraEnd) {
	char* paradelim = ">";
	char delim[] = " \t\r\n\v\f";
	char* reDir = NULL;
	char* reDirPos = NULL;
	reDirPos = strstr(*userLine, paradelim);
	char* left = NULL;
	char* right = NULL;
	if (reDirPos == NULL) {
		left = *userLine;
	}
	else {
		if(reDirPos == userLine[0]) {
			ehandler();
			return;
		}
		left = strtok(*userLine, paradelim);
		right = strtok(NULL, paradelim);
	}

	if(right != NULL) {
		if (strtok(NULL, paradelim) != NULL) {
			ehandler();
			return;
		}
		reDir = strtok(right, delim);
		if (reDir == NULL || strtok(NULL, delim)) {
			ehandler();
			return;
		}
	}

	char* firstCmd = strtok(left, delim);
	if (firstCmd == NULL) {
		return;
	}
	char* myargv[20];
	char* nextArg = strtok(NULL, delim);
	int counter = 1;
	while (nextArg != NULL) {
		myargv[counter++] = nextArg;
		nextArg = strtok(NULL, delim);
	}
	myargv[counter] = NULL;

	if (strcmp(firstCmd, "exit") == 0) {
		if (counter == 1) {
			exit(0);
		}
		else {
			ehandler();
		}
		return;
	}


	if (strcmp(firstCmd, "cd") == 0) {
		if (counter == 2) {
			chdir(myargv[1]);
		}
		else {
			ehandler();
		}
		return;
	}

	if (strcmp(firstCmd, "path") == 0) {
		free(path);
		path = malloc(sizeof(char*)*(counter));
		for(int i = 0; i < counter-1; i++) {
			path[i] = strdup(myargv[i+1]);
		}
		path[counter-1] = NULL;
		return;
	}
	int found = 0;
	int pathCounter = 0;
	while(path[pathCounter] != NULL) {
		char* result = malloc(strlen(path[pathCounter]) + strlen(firstCmd) + 2);
		strcpy(result, path[pathCounter]);
		strcat(result, "/");
		strcat(result, firstCmd);
		if(access(result, X_OK) == 0){
			found = 1;
			myargv[0] = firstCmd;
			int rc = fork();
			if (rc == 0) {
				if(reDir != NULL) {
					freopen(reDir, "w", stdout);
				}
				execv(result, myargv);
			}
			else {
				if (paraEnd == 1) {
					while(wait(NULL) > 0);
				}
				free(result);
				
			}
			break;
		}
		pathCounter++;
	}
	if (!found) {
		ehandler();
	}
}

int main(int argc, char* argv[]) {
	size_t n = 1;
	char* userLine = malloc(n);
	FILE* inputStream;
	path = malloc(sizeof(char*)*3);
	path[0] = strdup("/bin");
	path[1] = strdup("/usr/bin");
	path[2] = NULL;
	if(argc == 1) {
		inputStream = stdin;
	}
	if(argc == 2) {
		inputStream = fopen(argv[1], "r");
		if (inputStream == NULL) {
			ehandler();
			exit(1);
		}
	}
	if(argc > 2) {
		ehandler();
		exit(1);
	}

	while(1) {
		if (argc == 1) {
			printf("wish> ");
			fflush(stdout);
		}
		if(getline(&userLine, &n, inputStream)>=0) {
			if (strstr(userLine, "&") == NULL) {
				loopOnce(&userLine, &n, inputStream, 1);
				continue;
			}
			char* saveptr;
			char* token = strtok_r(userLine, "&", &saveptr);
			char* lastToken = NULL;
			while(token != NULL) {
				lastToken = token;
				token = strtok_r(NULL, "&", &saveptr);
				if (token != NULL) {
					loopOnce(&lastToken, &n, inputStream, 0);
				}
			}
			if (lastToken == NULL) {
				ehandler();
			}
			else {
				loopOnce(&lastToken, &n, inputStream, 1);
			}
			
		}
		else {
			break;
		}
	}
	exit(0);
}
