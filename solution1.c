/*
 *Author: Allen Suner
 *CSCI 322 Assignment 3
 *Solution #1
 *Febuary 3rd, 2014
 */

#include <stdio.h>
#include <string.h>
#include <mpi.h>
#include <stdlib.h>

#define INT_MAX 0x7FFFFFFF
#define INT_MIN 0x80000000

int main (int argc, char **argv) {
	/*SETUP*/
	double t1, t2, elapsed, global_time;
	int comm_sz, my_rank;
	int min = INT_MAX;
	int max = INT_MIN;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	unsigned int seed = my_rank;
	int message = rand_r(&seed)%100;
	unsigned int size = 1;
	usleep(rand_r(&seed)%100);
	
	MPI_Barrier(MPI_COMM_WORLD);
	t1 = MPI_Wtime();

	/*MESSAGE PASSING*/
	if (my_rank != 0) {
		//process i sends its value to process 0
		MPI_Send(&message, size, MPI_INT, 0, 0, MPI_COMM_WORLD);
		//process i receives the "min" from process 0
		MPI_Recv(&min, size, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		//process i receives the "max" from process 0
		MPI_Recv(&max, size, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	} else if (my_rank == 0) {
		int curr;
		int i;
		for (i = 1; i < comm_sz; i++) {
			//process 0 receives value from process i
			MPI_Recv(&curr, size, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			if (curr > max) {
				max = curr;
			}
			if (curr < min) {
				min = curr;
			}
		}
		//process 0 sends the values of "min" and "max" to process i
		for (i = 1; i < comm_sz; i++) {
			//"min" first
			MPI_Send(&min, size, MPI_INT, i, 0, MPI_COMM_WORLD);
			//"max" second
			MPI_Send(&max, size, MPI_INT, i, 0, MPI_COMM_WORLD);
		}
	}
	
	
	t2 = MPI_Wtime();
	elapsed = t2 - t1;
	MPI_Reduce(&elapsed, &global_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

	printf("I am process %d, and I took %f seconds to execute.\n", my_rank, global_time);

	/*TEARDOWN*/
	MPI_Finalize();
	return 0;
}
