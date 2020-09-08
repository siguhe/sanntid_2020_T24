#include <stdio.h>
#include "sort.h"

int main(int argc, char *argv[])
{
	printf("Arguments before sort: ");

	for (int i = 1; i < argc; i++) {
		printf("%s ", argv[i]);
	}
	printf("\n");

	sort(argc, argv);

	printf("Arguments after sort: ");

	for (int i = 1; i < argc; i++) {
		printf("%s ", argv[i]);
	}
	printf("\n");

	return 0;
}
