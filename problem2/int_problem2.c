#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <papi.h>
#include <errno.h>

#define EVENTS_NUM 4

int handle_error(int code) {
	printf ("Error: %d: %s\n", code, strerror(errno));
	return 1;
}

int** read_matrix_from_file(char * file, int * size) {
	int** matrix;
	char	type;
	FILE * f;
	f = fopen(file, "rb");
	fread(&type, sizeof(char), 1, f);
	fread(size, sizeof(int), 1, f);

	matrix = (int**)malloc(sizeof(int*) * *size);
	for (int i = 0; i < *size; i++)
		matrix[i] = malloc(sizeof(int) * *size);

	for (int i = 0; i < *size; i++)
		for (int j = 0; j < *size; j++)
			fread(&(matrix[i][j]), sizeof(int), 1, f);
	fclose(f);
	return matrix;
}

void write_matrix_in_file(char * file, int size, int** C) {
	FILE * f;
	f = fopen(file, "wb");
	char typeF = 'f';

	fwrite(&typeF, sizeof(char), 1, f);
	fwrite(&size, sizeof(int), 1, f);
	for (int i = 0; i < size; i++)
		for (int j = 0; j < size; j++)
			fwrite(&(C[i][j]), sizeof(int), 1, f);
	fclose(f);
}

int** multiply(int** A, int** B, int size, char* mode) {

	int** C = (int**)malloc(sizeof(int*) * size);
	for (int i = 0; i < size; i++)
		C[i] = malloc(sizeof(int) * size);

	if (!strcmp(mode, "0")) { //ijk
		for (int i = 0; i < size; i++)
			for (int j = 0; j < size; j++)
				for (int k = 0; k < size; k++)
					C[i][j] += A[i][k] * B[k][j];
		printf("multiply mode == ijk\n");
	}

	else if (!strcmp(mode, "1")) { //ikj
		for (int i = 0; i < size; i++)
			for (int k = 0; k < size; k++)
				for (int j = 0; j < size; j++)
					C[i][j] += A[i][k] * B[k][j];
		printf("multiply mode == ikj\n");
	}

	else if (!strcmp(mode, "2")) { //kij
		for (int k = 0; k < size; k++)
			for (int i = 0; i < size; i++)
				for (int j = 0; j < size; j++)
					C[i][j] += A[i][k] * B[k][j];
		printf("multiply mode == kij\n");
	}

	else if (!strcmp(mode, "3")) { //jik
		for (int j = 0; j < size; j++)
			for (int i = 0; i < size; i++)
				for (int k = 0; k < size; k++)
					C[i][j] += A[i][k] * B[j][k];
		printf("multiply mode == jik\n");
	}

	else if (!strcmp(mode, "4")) { //jki
		for (int j = 0; j < size; j++)
			for (int k = 0; k < size; k++)
				for (int i = 0; i < size; i++)
					C[i][j] += A[i][k] * B[k][j];
		printf("multiply mode == jki\n");
	}

	else if (!strcmp(mode, "5")) { //kji
		for (int k = 0; k < size; k++)
			for (int j = 0; j < size; j++)
				for (int i = 0; i < size; i++)
					C[i][j] += A[i][k] * B[k][j];
		printf("multiply mode == kji\n");
	}

	return C;
}

int main(int argc, char **argv) { 
	int			size;
	int     	**A, **B, **C;
	int     	event_set = PAPI_NULL;
	int   		event_codes[EVENTS_NUM] = {PAPI_L1_DCM, PAPI_L1_ICM, PAPI_L1_LDM, PAPI_L1_STM};
	long long  	papi_values[EVENTS_NUM];

	if (argc != 5) {
		fprintf(stderr, "Less of arguments\n");
		return 1;
	}
	A = read_matrix_from_file(argv[1], &size);
	B = read_matrix_from_file(argv[2], &size);
	printf("\ntype of elements == int\n");
	if (PAPI_library_init(PAPI_VER_CURRENT) != PAPI_VER_CURRENT)
		return handle_error(0);
	if (PAPI_create_eventset(&event_set) != PAPI_OK)
		return handle_error(1);
	if (PAPI_add_events(event_set, event_codes, EVENTS_NUM) != PAPI_OK)
		return handle_error(2);
	if (PAPI_start(event_set) != PAPI_OK )
		return handle_error(3);
	C = multiply(A, B, size, argv[4]);
	if (PAPI_stop(event_set, papi_values) != PAPI_OK)
		return handle_error(4);
	if (PAPI_remove_events(event_set, event_codes, EVENTS_NUM) != PAPI_OK)
		return handle_error(5);
	if (PAPI_destroy_eventset(&event_set) != PAPI_OK)
		return handle_error(6);
	PAPI_shutdown();
	printf("L1 data + instructions misses == %lld\n", papi_values[0] + papi_values[1]);
	printf("L1 load misses == %lld\n", papi_values[2]);
	printf("L1 store misses == %lld\n", papi_values[3]);
	for (int i = 0; i < size; i++)
		free(A[i]);
	free(A);
	for (int i = 0; i < size; i++)
		free(B[i]);
	free(B);
	for (int i = 0; i < size; i++)
		free(C[i]);
	free(C);
	return 0;
}
