//----------------------------------------------------------------------------- 
// Program code for CS 415/515 Parallel Programming, Portland State University.
//----------------------------------------------------------------------------- 

// The sum program (MPI version).
//
//
#include <stdio.h>
#include <mpi.h>

#define N 1000 	/* problem parameter */

int compute(int i) { return i*i; }

int main(int argc, char **argv) {  
  int rank, size;
  MPI_Init(&argc, &argv);  
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);  
  MPI_Comm_size(MPI_COMM_WORLD, &size);  
  
  int low = (N/size) * rank;    /* a simplistic partition scheme */ 
  int high = low + (N/size);	
  int psum = 0;
  for (int i = low; i < high; i++)
    psum += compute(i);

  int sum=0, dest=0;
  MPI_Reduce(&psum, &sum, 1, MPI_INT, MPI_SUM, dest, MPI_COMM_WORLD);
  if (rank == dest)
    printf("The sum is %d (should be 332833500)\n", sum);

  MPI_Finalize();
}  
