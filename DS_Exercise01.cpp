#include <iostream>
#include <cstdlib>
#include <mpi.h>

int world_size;
int world_rank;

int main(int argc, char** argv) {
  MPI_Init(NULL, NULL);
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

  if (world_size != 4){
    if (world_rank == 0)
    std::cout << "Error : You must use 4 processors" << std::endl;
  }
  else {
    if(world_rank == 0)
      std::cout << "Student name: de Bellescize François" << std::endl << "Student number: 2927126" << std::endl;
  }

  MPI_Finalize();
  return 0;
}
