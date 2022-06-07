//-----------------------------------------------------------------------------
// Program code for CS 415/515 Parallel Programming, Portland State University.
//-----------------------------------------------------------------------------

// A demo program of MPI concurrent I/O, with file view settings.
//
// Usage:
//   linux> mpirun -n <#procs> file-view <infile> <outfile>
//
#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

int compute(int i) { return i*i; }

int main(int argc, char *argv[])
{
  int rank, P, offset, buf[8];
  MPI_File fin, fout;
  MPI_Status st;
  MPI_Offset fsize;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &P);

  if (argc < 3) {
    if (rank == 0)
      printf("Usage: mpirun -n <#procs> file-view <infile> <outfile>\n");
    MPI_Finalize();
    return 0;
  }


  // all processes open the same pair of files
  MPI_File_open(MPI_COMM_WORLD, argv[1], MPI_MODE_RDONLY, MPI_INFO_NULL, &fin);
  MPI_File_get_size(fin, &fsize);
  int length = fsize/P;
  printf("length is %d\n",length);
  if ((length % P) != 0)
  {
          MPI_Finalize();
    return 0;
  }

int temp = (fsize/sizeof(int))/P;
  int arr[temp];
  printf("temp = %d\n",temp);
  printf("size of the array %d\n", (int) sizeof(arr));
  offset = rank * temp * sizeof(int);
  printf("Offset for rank %d = %d\n",rank,offset);
  MPI_File_set_view(fin, offset, MPI_INT, MPI_INT, "native", MPI_INFO_NULL);
  MPI_File_read(fin, arr, temp, MPI_INT, &st);
  int psum = 0;
  for (int i = 0; i < temp; i++) {
    psum += arr[i];
  }
  printf("rank %d psum = %d\n",rank,psum);
  //printf("rank=%d: buf=[%d,%d,%d,%d,%d,%d,%d,%d] and its psum = %d\n", rank, arr[0], arr[1], arr[2], arr[3], arr[4], arr[5], arr[6], arr[7], psum);

  int sum=0, dest=0;
  MPI_Reduce(&psum, &sum, 1, MPI_INT, MPI_SUM, dest, MPI_COMM_WORLD);
  if (rank == dest)
    printf("The sum is %d\n", sum);

  MPI_File_open(MPI_COMM_WORLD, argv[2],
                MPI_MODE_CREATE|MPI_MODE_WRONLY, MPI_INFO_NULL, &fout);


  // each writes four integers, to a specific position
  offset = rank * temp * sizeof(int);
  MPI_File_set_view(fout, offset, MPI_INT, MPI_INT, "native", MPI_INFO_NULL);
  for (int i = 0; i < temp; i++) {
    arr[i] = arr[i] * 2;
  }
  MPI_File_write(fout, arr, temp, MPI_INT, &st);

  MPI_File_close(&fin);
  MPI_File_close(&fout);
  MPI_Finalize();
}