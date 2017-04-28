#include <iostream>
#include <mpi.h>

int ARRAY_SIZE = 120000;
int MASTER = 1;
int SEND_SUM = 1;

void coordinator(int world_rank, int world_size) {
  int values[ARRAY_SIZE];
  int partition_size = ARRAY_SIZE / (world_size - 1);

  // Initialise the array with random values
  srand(time(NULL));
  for(unsigned int i = 0; i < ARRAY_SIZE; i++)
    values[i] = rand() % 101;

  // Send to each slave the partition size then a partition
  MPI_Bcast(&partition_size, 1, MPI_INT, MASTER, MPI_COMM_WORLD);
  int offset = 0;
  for(unsigned int i = 0; i < world_size; i++) {
    if (i != MASTER){
      MPI_Send(values + partition_size * offset, partition_size, MPI_INT, i, 0, MPI_COMM_WORLD);
      ++offset;
    }
  }

  // Calc the total of each node
  int total = 0;
  int total_node;
  for(int i = 0; i < world_size; i++) {
    if (i != MASTER){
      MPI_Send(&SEND_SUM, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
      MPI_Recv(&total_node, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      total += total_node;
    }
  }
  std::cout << "Total : " << total << std::endl;
}

void participant(int world_rank, int world_size) {

  // Get the size of the partition
  int partition_size = 0;
  MPI_Bcast(&partition_size, 1, MPI_INT, 1, MPI_COMM_WORLD);

  // Set the size of the partition
  int *partition = new int[partition_size];
  MPI_Recv(partition, partition_size, MPI_INT, MASTER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

  // Calc the half sum of the partition
  int total = 0;
  int offset;
  if (world_rank % 2 == 0)
    offset = partition_size / 2;
  else
    offset = 0;

  for(unsigned int i = offset; i < offset + (partition_size / 2); i++)
    total += partition[i];
  std::cout << "Total for node " << world_rank << " : " << total << std::endl;

  // Wait the message to send the total
  int message_type;
  MPI_Recv(&message_type, 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  while(message_type != SEND_SUM) {
    MPI_Recv(&message_type, 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }

  // Send the total
  MPI_Send(&total, 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD);

  delete[] partition;
}

int main(int argc, char** argv) {
  int world_size;
  int world_rank;

  MPI_Init(NULL, NULL);
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

  if (world_size != 6){
    if (world_rank == 0)
    std::cout << "Error : You must use 6 processors" << std::endl;
  }
  else {
    if(world_rank == MASTER)
    coordinator(world_rank, world_size);
    else
    participant(world_rank, world_size);
  }
  MPI_Finalize();
}
