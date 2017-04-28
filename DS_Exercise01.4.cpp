#include <iostream>
#include <mpi.h>

int N = 10000000;
int MASTER = 0;

int main(int argc, char **argv) {
  int world_size, world_rank;

  MPI_Init(NULL, NULL);
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

  if (N % world_size != 0){
    if (world_rank == 0)
      std::cout << "The array size (" << N << ") is not divisible by the number of node : " << world_size << std::endl;
  }
  else{
    int size = N / world_size;
    int offset = world_rank * size;
    double total, h, sum, x;

    // Calucul the part of the sum depending of the world_rank
    h = 1.0 / (double) N;
    sum = 0.0;
    for (int i = offset; i <= offset + size; i++) {
      x = h * ((double)i - 0.5);
      sum += 4.0 / (1.0 + x*x);
    }

    std::cout << "Node[" << world_rank << "] Sum : " << sum << std::endl;

    if (world_rank == 0) {

      // Receive data from other nodes
      total += sum;
      for (int i = 0; i < world_size; i++){
        double data;

        if (i != MASTER){
          std::cout << "Node[" << world_rank << "] get sum from node[" << i << "] ... ";
          MPI_Recv(&data, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
          std::cout << " Sum receive : " << data << std::endl;
          total += data;
        }
      }
      std::cout << "PI = " << total / N << std::endl;
    } else{
      // Send data to master
      MPI_Send(&sum, 1, MPI_DOUBLE, MASTER, 0, MPI_COMM_WORLD);
    }
  }

  MPI_Finalize();
}
