#include <iostream>
#include <mpi.h>

int ARRAY_SIZE = 200;

int main(int argc, char **argv) {
  int world_size, world_rank;

  MPI_Init(NULL, NULL);
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

  if (ARRAY_SIZE % world_size != 0){
    if (world_rank == 0)
      std::cout << "The array size (" << ARRAY_SIZE << ") is not divisible by the number of node : " << world_size << std::endl;
  }
  else{
    int *total_array = new int[ARRAY_SIZE];
    int *partition = new int[ARRAY_SIZE / world_size];

    srand(world_rank);
    std::cout << "rank " << world_rank << " numbers: ";

    // Initialize array with random numbers
    for(unsigned int i = 0; i < ARRAY_SIZE / world_size; i++) {
      partition[i] = rand() % 10;
      std::cout << partition[i] << ", ";
    }
    std::cout << std::endl;

    // Gathers values from a group of processes
    MPI_Gather(partition, ARRAY_SIZE / world_size, MPI_INT, total_array, ARRAY_SIZE / world_size, MPI_INT, 0, MPI_COMM_WORLD);
    if(world_rank == 0) {
      std::cout << std::endl << "-------- total data --------" << std::endl;
      for(unsigned int i = 0; i < ARRAY_SIZE; i++) {
        std::cout << total_array[i] << ", ";
        if(i % 20 == 19)
        std::cout << std::endl;
      }
      std::cout << std::endl;
    }

    delete[] total_array;
    delete[] partition;
  }

  MPI_Finalize();
}
