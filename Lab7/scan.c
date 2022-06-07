//----------------------------------------------------------------------------- 
// Program code for CS 415/515 Parallel Programming, Portland State University.
//----------------------------------------------------------------------------- 

// MPI scan routine.
//
#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>

int main(int argc, char **argv) {
  int rank, size, *vector; 

  MPI_Init(&argc, &argv);  
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);  
  MPI_Comm_size(MPI_COMM_WORLD, &size); 

  if (rank == 0) {
    vector = (int *) malloc(sizeof(int) * size);
  } 

  // sum-scan the rank numbers
  int psum;
  MPI_Scan(&rank, &psum, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
  printf("P[%d] psum = %d\n", rank, psum);
  int *mypsum = (int *) malloc(sizeof(int));
  mypsum = &psum;


  MPI_Gather(mypsum, 1, MPI_INT, vector, 1, MPI_INT, 0, MPI_COMM_WORLD);

  // /* The root process prints out the result */
  if (rank == 0) {
    for (int i = 0; i < size; i++) {
      printf("From Root [%d] => %d", i, vector[i]);
      printf("\n");
    }
  }

  MPI_Finalize();
}  
