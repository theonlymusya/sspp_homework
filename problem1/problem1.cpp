#include <iostream>
#include <vector>
#include <cstring>
#include <cerrno>
#include <sys/time.h>

template <class T>
void matr_input(FILE * fp, std::vector <std::vector <T> > &matr) {
	int N;
	fread(&N, sizeof(int), 1, fp);
	matr.resize(N);
	for (int i = 0; i < N; i++) {
		matr[i].resize(N);
		for (int j = 0; j < N; j++)
            fread(&(matr[i][j]), sizeof(T), 1, fp);
	}
}

template <class T>
void matr_output(char * file, size_t N, std::vector <std::vector <T> > &matr_C, char type) {

	FILE *fp;
	if ((fp = fopen(file, "wb")) == nullptr) {
		std::cout << __FILE__ << "(" << __LINE__ - 2 << "): Can't open file for output: "
             << strerror(errno) << std::endl;
        return;
    }
	fwrite(&type, sizeof(char), 1, fp);
	fwrite(&N, sizeof(int), 1, fp);
	for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            fwrite(&(matr_C[i][j]), sizeof(T), 1, fp);
        }
    }
	fclose(fp);
}

template <class T>
void mult_exec(char * mode, char * file_name, std::vector <std::vector <T> > &A, std::vector <std::vector <T> > &B, char type) {

	std::vector <std::vector <T> > C;

	C.resize(A.size());
	for (int i = 0; i < C.size(); i++)
		C[i].resize(A.size());
	//размеры результирующей матрицы равны размерам входных

	if (!strcmp(mode, "0")) { //ijk
		printf("Multiply mode ijk\n");
		for (int i = 0; i < C.size(); i++)
			for (int j = 0; j < C.size(); j++)
				for (int k = 0; k < C.size(); k++)
					C[i][j] += A[i][k] * B[k][j];
	}

	else if (!strcmp(mode, "1")) { //ikj
		printf("Multiply mode ikj\n");
		for (int i = 0; i < C.size(); i++)
			for (int k = 0; k < C.size(); k++)
				for (int j = 0; j < C.size(); j++)
					C[i][j] += A[i][k] * B[k][j];
	}

	else if (!strcmp(mode, "2")) { //kij
		printf("Multiply mode kij\n");
		for (int k = 0; k < C.size(); k++)
			for (int i = 0; i < C.size(); i++)
				for (int j = 0; j < C.size(); j++)
					C[i][j] += A[i][k] * B[k][j];
	}

	else if (!strcmp(mode, "3")) { //jik
		printf("Multiply mode jik\n");
		for (int j = 0; j < C.size(); j++)
			for (int i = 0; i < C.size(); i++)
				for (int k = 0; k < C.size(); k++)
					C[i][j] += A[i][k] * B[j][k];
	}

	else if (!strcmp(mode, "4")) { //jki
		printf("Multiply mode jki\n");
		for (int j = 0; j < C.size(); j++)
			for (int k = 0; k < C.size(); k++)
				for (int i = 0; i < C.size(); i++)
					C[i][j] += A[i][k] * B[k][j];
	}

	else if (!strcmp(mode, "5")) { //kji
		printf("Multiply mode kji\n");
		for (int k = 0; k < C.size(); k++)
			for (int j = 0; j < C.size(); j++)
				for (int i = 0; i < C.size(); i++)
					C[i][j] += A[i][k] * B[k][j];
	}

	matr_output(file_name, C.size(), C, type);
}

FILE * open_file (char * filename) {
    FILE * fd;
    if ((fd = fopen(filename, "rb")) == nullptr) {
        std::cout << __FILE__ << "(" << __LINE__ - 2 << "): Can't open file: "
                  << strerror(errno) << std::endl;
        exit(1);
    }
    return fd;
}

void close_file (FILE * fd) {
    if (fclose(fd) == EOF) {
        std::cout << __FILE__ << "(" << __LINE__ - 2 << "): Can't close file 2: "
                  << strerror(errno) << std::endl;
        exit(1);
    }
}

int main(int argc, char *argv[]) {

	char * filename;
    FILE * fd;
    char matr_elem_type;
    struct timeval t_start, t_end;

	if (argc < 5) {
		std::cout << "Less of arguments" << std::endl;
		exit(1);
	}
    filename = argv[1];
    fd = open_file (filename);
    fread(&matr_elem_type, sizeof(char), 1, fd);

	if (matr_elem_type == 'd') {

		printf("Matrices elements type : int\n");
        std::vector <std::vector <int> > matr_A;
        std::vector <std::vector <int> > matr_B;
        matr_input(fd, matr_A);

        close_file(fd);
        filename = argv[2];
        fd = open_file (filename);
        fread(&matr_elem_type, sizeof(char), 1, fd);

        matr_input(fd, matr_B);

        close_file(fd);

        gettimeofday(&t_start, nullptr);
        mult_exec(argv[4], argv[3], matr_A, matr_B, matr_elem_type);
        gettimeofday(&t_end, nullptr);

    } else if (matr_elem_type == 'l') {

    	printf("Matrices elements type : long long\n");
        std::vector <std::vector <long long> > matr_A;
        std::vector <std::vector <long long> > matr_B;
        matr_input(fd, matr_A);

        close_file(fd);
        filename = argv[2];
        fd = open_file (filename);
        fread(&matr_elem_type, sizeof(char), 1, fd);

        matr_input(fd, matr_B);

        close_file(fd);

        gettimeofday(&t_start, nullptr);
        mult_exec(argv[4], argv[3], matr_A, matr_B, matr_elem_type);
        gettimeofday(&t_end, nullptr);
    }

    float elapsed_time;
    elapsed_time = (float)(t_end.tv_usec - t_start.tv_usec) / 1000000.0 + (float)(t_end.tv_sec - t_start.tv_sec);
    printf("Elapsed time = %lf s\n", elapsed_time);
	return 0;
}
