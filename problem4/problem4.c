#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

void print_vector(char * file, int size, long long * vector) {
	FILE * fd;
	fd = fopen(file, "wb");
	fwrite(&size, sizeof(int), 1, fd);
	for (long long i = 0; i < size; i++)
		fwrite(&(vector[i]), sizeof(long long), 1, fd);
	fclose(fd);
}

long long* read_vector(char * file, int * size) {
	long long* vector;
	FILE * fd;
	fd = fopen(file, "rb");
	fread(size, sizeof(int), 1, fd);
	vector = (long long*)malloc(sizeof(long long*) * (*size));
	for (int i = 0; i < (*size); i++)
		fread(&(vector[i]), sizeof(long long), 1, fd);
	fclose(fd);
	return vector;
}

long long** read_matrix(char * file, int * size) {
	long long** matrix;
	FILE * fd;
	fd = fopen(file, "rb");
	fread(size, sizeof(int), 1, fd);

	matrix = (long long**) malloc(sizeof(long long*) * (*size));
    for (int i = 0; i < (*size); i++) {
        matrix[i] = malloc(sizeof(long long) * (*size));
	}
	for (int i = 0; i < (*size); i++)
		for (int j = 0; j < (*size); j++)
			fread(&(matrix[i][j]), sizeof(long long), 1, fd);

	fclose(fd);
	return matrix;
}

int main(int argc, char **argv)
{
	int size, rank;
	int i, j;
	assert((argc == 4) && "Not enough arguments");
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Status status;
	MPI_Barrier(MPI_COMM_WORLD);
	int matrix_size, vector_size;
	double time_start, time_end, elapsed_time;
	if (rank == 0) {
		long long ** matrix = read_matrix(argv[1], &matrix_size);
		long long * vector = read_vector(argv[2], &vector_size);

		assert((matrix_size == vector_size)&&"Invalid sizes");
		time_start = MPI_Wtime();
		long long result[matrix_size];
		int count_of_rows_arr[size + 1];
		count_of_rows_arr[0] = 0;
		int tmp_size = matrix_size;
		for (int i = 0; i < size; i++) {
			count_of_rows_arr[i + 1] = count_of_rows_arr[i] + tmp_size / (size - i);
			tmp_size -= tmp_size / (size - i);
		}
		for (i = 1; i < size; i++) {
			MPI_Send(&matrix_size, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			
			for (int j = count_of_rows_arr[i]; j < matrix_size && j < count_of_rows_arr[i + 1]; j++) {
				MPI_Send(matrix[j], matrix_size, MPI_LONG_LONG, i, 2, MPI_COMM_WORLD);
			}
			MPI_Send(vector, matrix_size, MPI_LONG_LONG, i, 3, MPI_COMM_WORLD);
		}

		for (i = 0; i < matrix_size && i < count_of_rows_arr[1]; i++) {
			result[i] = 0;
			for (j = 0; j < matrix_size; j++)
				result[i] += matrix[i][j] * vector[j];
		}
		
		for (i = 1; i < size; i++) {
			long long local_result[matrix_size];
			MPI_Recv(local_result, matrix_size, MPI_LONG_LONG, i, 4, MPI_COMM_WORLD, &status);
			for (j = count_of_rows_arr[i]; j < matrix_size && j < count_of_rows_arr[i + 1]; j++)
				result[j] = local_result[j];
		}

		time_end = MPI_Wtime();
		double max_time = time_end - time_start;
		printf("Time of %d process = %lf\n", rank, time_end - time_start);

		for (int i = 1; i < size; i++) {
			double time;
			MPI_Recv(&time, 1, MPI_DOUBLE, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);
			if (time > max_time)
				max_time = time;
		}
		printf("Max time = %lf\n", max_time);
		print_vector(argv[3], matrix_size, result);

		for (int i = 0; i < (int)matrix_size; i++) {
			free(matrix[i]);
		}
		free(matrix);
		free(vector);
	} 
	else {

		time_start = MPI_Wtime();
		MPI_Recv(&matrix_size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		int count_of_rows_arr[size + 1];
		count_of_rows_arr[0] = 0;
		int tmp_size = matrix_size;
		for (int i = 0; i < size; i++) {
			count_of_rows_arr[i + 1] = count_of_rows_arr[i] + tmp_size / (size - i);
			tmp_size -= tmp_size / (size - i);
		}
		long long matrix[count_of_rows_arr[rank + 1] - count_of_rows_arr[rank]][(int)matrix_size];
		for (int i = 0; i < (count_of_rows_arr[rank + 1] - count_of_rows_arr[rank]); i++) 
			MPI_Recv(matrix[i], (int)matrix_size, MPI_LONG_LONG, 0, 2, MPI_COMM_WORLD, &status);
		long long vector[(int)matrix_size];

		MPI_Recv(vector, (int)matrix_size, MPI_LONG_LONG, 0, 3, MPI_COMM_WORLD, &status);
		long long result[(int)matrix_size];
		time_start = MPI_Wtime();
		for (int i = count_of_rows_arr[rank]; i < (int)matrix_size && i < count_of_rows_arr[rank + 1]; i++) {
			result[i] = 0;
			for (j = 0; j < (int)matrix_size; j++)
				result[i] += matrix[i - count_of_rows_arr[rank]][j] * vector[j];
		}
		
		MPI_Send(result, (int)matrix_size, MPI_LONG_LONG, 0, 4, MPI_COMM_WORLD);
		time_end = MPI_Wtime();
		elapsed_time = time_end - time_start;
		MPI_Send(&elapsed_time, 1, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD);
		printf("Time of %d process = %lf\n", rank, elapsed_time);
	}

	MPI_Finalize();
	return 0;
}
