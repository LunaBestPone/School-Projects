#include <stdio.h>

int array[100000];
int main() {
	for (int i = 0; i < 100000; i++) {
		array[i] = i;
	}
	return 0;
}
