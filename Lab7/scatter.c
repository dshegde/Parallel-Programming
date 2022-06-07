//----------------------------------------------------------------------------- 
// Program code for CS 415/515 Parallel Programming, Portland State University.
//----------------------------------------------------------------------------- 

// MPI scatter routine.
//
#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>

int main(int argc, char **argv) {
  int rank, size; 

  MPI_Init(&argc, &argv);  
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);  
  MPI_Comm_size(MPI_COMM_WORLD, &size);  
  
  // process 0 prepares an array of data
  int *cnt;
  if (rank == 0) {
    cnt = (int *) malloc(sizeof(int) * size);
    for (int i = 0; i < size; i++)
      cnt[i] = 2 * i;
  }

  // scatter the data to all processes
  int mycnt;
  MPI_Scatter(cnt, 1, MPI_INT, &mycnt, 1, MPI_INT, 0, MPI_COMM_WORLD);
  printf("P[%d] mycnt = %d\n", rank, mycnt);

  int *data, *disp, rbuf[2 * size];
  if (rank == 0) {
    data = (int *) malloc(sizeof(int) * 128); // source data
    disp = (int *) malloc(sizeof(int) * size); // displacement array
  }

  if (rank == 0) {
    for (int i=0; i<128; i++) {
      data[i] = i;
    }
  }

  if (rank == 0) {
    disp[0] = 0;
    for (int i=1; i<size; i++) {
      disp[i] = disp[i-1] + (i-1) * 2;
    }
  }

  // scatter variable-sized data sections to all processes
  MPI_Scatterv(data, cnt, disp, MPI_INT, rbuf, 2*size, MPI_INT, 0, MPI_COMM_WORLD);

  printf("P[%d] got %d items: ", rank, mycnt);
  for (int i=0; i < mycnt; i++) {
    printf("%d,", rbuf[i]);
  }
  printf("\n");

  MPI_Finalize();
}  
