//-----------------------------------------------------------------------------
// Program code for CS 415P/515 Parallel Programming, Portland State University
//-----------------------------------------------------------------------------

// Bucket sort (sequential version)
//
// Usage:
//   linux> ./bsort B [N]
//   -- B (#buckets) must be a power of 2; B defaults to 10
//
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <mpi.h>

#define TAG 1001
#define DEBUG
#define DATABITS 13     // assume data are 13-bit integers: [0,8191]

// return true if x is a power of 2
#define IsPowerOf2(x) (!((x) & ((x) - 1)))

// bucket index for integer x of b bits (B is #buckets)
#define BktIdx(x,b,B) ((x) >> ((b) - (int)log2(B)))

#define MAX(x, y) (((x) > (y)) ? (x) : (y))

int rank, numcount, N, P, offset;
MPI_Status st;
int totalcount = 0;
int startidx;


// Print array
void print_array(int *a, int n) {
  for (int i = 0; i < n; i++)
    printf("%4d ", a[i]);
  printf("\n");

}
// Initialize array with random 13-bit int values
// (except if WORST flag is on, set array to the reverse of 1..N)
void init_array(int *a, int n) {
#ifdef WORST
  for (int i = 0; i < n; i++)
    a[i] = n - i + 1;
#else
  srand(time(NULL));
  for (int i = 0; i < n; i++)
    a[i] = rand() % 8192;
#endif
#ifdef DEBUG
  printf("Init (%d elements): ", n);
  print_array(a, n);
#endif
}

// Verify that array is sorted (and report error if exits)
void verify_array(int *a, int n) {
  for (int i = 0; i < n-1; i++)
    if (a[i] > a[i+1]) {
      printf("FAILED: a[%d]=%d, a[%d]=%d\n", i, a[i], i+1, a[i+1]);
      return;
    }
  printf("%d element array is sorted.", n);
}

// Bubble sort
//
void bubble_sort(int *a, int n) {
  for (int i = 0; i < n; i++)
    for (int j = i+1; j < n; j++)
      if (a[i] > a[j]) {
        int tmp = a[i];
        a[i] = a[j];
        a[j] = tmp;
      }
}
// Bucket sort
//
void bucket_sort(int *a, int n, int num_buckets) {
  // allocate buckets with a safe bucket size
  int buckarrsize = MAX(2*n/num_buckets, 4);
  int bucket[num_buckets][buckarrsize];


  // individual bucket count

  int bcnt[num_buckets];

  for (int k = 0; k < num_buckets; k++)
    bcnt[k] = 0;



  // distribute data to buckets
  for (int i = 0; i < numcount; i++) {
    int k = BktIdx(a[i], DATABITS, num_buckets);
    bucket[k][bcnt[k]++] = a[i];
  }


#ifdef DEBUG
  for (int k = 0; k < num_buckets; k++) {
    printf("bucket[%d]: ", k);
    print_array(bucket[k], bcnt[k]);
  }
#endif

  for (int i=0; i<num_buckets; i++)
  {
          if (rank != i)
          {
                  int arraysize = bcnt[i];
                  MPI_Send(bucket[i], arraysize, MPI_INT, i, TAG, MPI_COMM_WORLD);
                  //printf("sending bucket[%d] to rank %d   ---- current rank = %d\n",i, i, rank);
          }


  }
int merged_bucket[2*numcount];
  int idx = 0;
  int recvbuf[2*numcount];
  for (int i=0; i<num_buckets; i++)
  {
          if (rank != i)
          {
                  int count = 0;
                  MPI_Recv(recvbuf, numcount, MPI_INT, i, MPI_ANY_TAG, MPI_COMM_WORLD, &st);
                  MPI_Get_count(&st, MPI_INT, &count);
                  //printf("Received bucket[%d] from rank %d and Count = %d\n",rank, i, count);
                  totalcount += count;
                  for (int j = 0; j < count; j++) {
                        merged_bucket[idx++] = recvbuf[j];
                  }
          }
          else
          {
                  int *temp = bucket[rank];
                  totalcount += bcnt[rank];
                  for (int j=0; j<bcnt[rank]; j++)
                  {
                          merged_bucket[idx++] = temp[j];
                  }
          }
  }


  // bubble sort bucket
  bubble_sort(merged_bucket, totalcount);
  printf("After sorting ------------ This array is for rank = %d\n [",rank);
  for (int i=0; i<totalcount; i++)
  {
          printf("%d, ",merged_bucket[i]);
  }
  printf("]\n");

  MPI_Scan(&totalcount, &startidx, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
  startidx = startidx - totalcount;
  //printf("P[%d] startidx = %d\n", rank, startidx);

  // copy sorted data back to array a
  int k = 0;
    for (int j = 0; j < totalcount; j++)
      a[k++] = merged_bucket[j];
}
// Main routine
//
int main(int argc, char **argv) {

  MPI_File fin, fout;
  MPI_Offset fsize;
  int insert_offset;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &P);

  if (argc < 3) {
    printf("Usage: mpirun -n P bsort-mpi <infile> <outfile>\n");
    MPI_Finalize();
    return 0;
  }

  if (!IsPowerOf2(P)) {
    printf("P (#buckets) must be a power of 2\n");
    MPI_Finalize();
    return 0;
  }


  // all processes open the same pair of files
  MPI_File_open(MPI_COMM_WORLD, argv[1], MPI_MODE_RDONLY, MPI_INFO_NULL, &fin);
  MPI_File_open(MPI_COMM_WORLD, argv[2], MPI_MODE_CREATE|MPI_MODE_WRONLY, MPI_INFO_NULL, &fout);
  MPI_File_get_size(fin, &fsize);
  //printf("Input file size is %lld\n",fsize);
  N = fsize/sizeof(int);
  //printf("Data size of the file = %d\n",N);

  if ((N%P) != 0)
  {
        MPI_Finalize();
        return 0;
  }
 numcount = N/P;
  //printf("Array size is %d\n",numcount);

  int array[numcount];
  offset = rank * numcount * sizeof(int);
  MPI_File_set_view(fin, offset, MPI_INT, MPI_INT, "native", MPI_INFO_NULL);
  MPI_File_read(fin, array, numcount, MPI_INT, &st);

  bucket_sort(array, numcount, P);

  insert_offset = startidx * sizeof(int);
  MPI_File_set_view(fout, insert_offset, MPI_INT, MPI_INT, "native", MPI_INFO_NULL);
  MPI_File_write(fout, array, totalcount, MPI_INT, &st);
  MPI_File_close(&fin);
  MPI_File_close(&fout);
  MPI_Finalize();

}