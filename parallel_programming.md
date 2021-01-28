# Overview
1. Parallel Computing
- Using multiple computer resources simultaneously
- A problem can be broken into a number of parts that can be executed in parallel
- Computer resources:
	+ A single computer with multiple processors
	+ Multiple computers connected to each other by a network
	+ Combination of both
- Node:
	+ CPUs/processors/cores
- Task:
	+ A program or set of instructions that executed by a processor
	+ A parallel program consists of multiple tasks running on multiple processors
- Shared memory:
	+ All processors have direct (bus based) access to common physical memory
- Symmetric multi-processor (SMP):
	+ Hardware architecture where multiple processors share a single address space and access to all resources
- Distributed memory:
	+ Hardware: Network based memory access for physical memory
	+ Programming model: tasks must use communication to access memory on other machines where other task are executing
- Communication:
	+ Parallel tasks need to exchange data (shared memory bus or via network)
- Synchronization:
	+ A point where one task may not proceed further until other tasks reach the same point
	+ Invole waiting, therefore the parallel application's wall-clock may increase
- Observered Speedup: `wall-clock time of serial execution` / `wall-clock time of parallel execution`
- Parallel overhead:
	+ Task start-up time
	+ Synchronizations
	+ Data communications
	+ Software overhead imposed by parallel compilers, libraries, tools, operating system,...
	+ Task termination time
- Scalability:
	+ Refers to a parallel system's ability to increase speedup with addition of more processors
2. Parallel Computer Memory Architecture
- Shared memory:
	+ All processors can access to all memory as global address space
	+ Multi processors operate independenly
	+ Changes in memory effected by one processor are visible to all other processors
	+ 2 classes:
		. UMA (Uniform memory access)
			. All processors know about an update to a location in shared memory made by one processor
			. Accomplish at the hardware level
			. Common: SMP (Symmetric MultiProcessor)
		. NUMA (Non-uniform memory access)
			. Physically link two or more SMPs
			. Each SMP can access directly to memory of another
			. Processors do not have indetical access and access time to all memory
			. Memory access across link is slower
			. If cache coherency is maintained: CC-NUMA
		-> Advantages:
			. Data sharing is fast and uniform
		-> Disadvantages:
			. Lack of scalability of memory and processors: adding more memory and processors increase traffic on the shared memory-processors path, and for cache coherent system
			. Programmer responsibility for synchronization
			. Expensive to design and produce shared memory machines with ever increasing number of processors
- Distributed memory:
	+ Based on communication network
	+ Processors has their own local memory, no global address space across all processors
	+ Processors oparate independently, changes of a local memory of a processor do not affect on the memory of other processors
	+ Communication is based on data transfer
	+ Network fabric used for data transfer is usually Ethernet
	-> Advantages
		. Each processor can access  rapidly to its own local memory without interference and without overhead incurred when trying to maintain cache coherency
		. Cheap for extending
	-> Disadvantages:
		. Difficult to map existing data structures from global memory to local memory
		. Programmer responsibility for exchanging data
		. Non-uniform memory access
- Hybrid Distributed-Shared memory
3. Parallel Programming Models
- Thread model
	+ A single process can have multiple concurent execution paths
	+ A program (main routine of the application) performs some serial work, and then creates number of tasks (threads) that can be scheduled and run by the operating system concurrently
	+ Each thread has local data, but also shares the entire resources of the program
	+ A program has subroutines and any thread can execute any subroutine
	+ A thread can communicate with other threads through global memory. This requires synchronization constructs to ensure that more than one thread cannot update the same global address at any time
	+ Thread can come and go but the main routine remains present to provide the neccessary shared resources until the application has compleleted
	+ Implementation:
		. POSIX Thread
		. OpenMP
- Distributed memory / Memory passing model
	+ A set of tasks use their own local memory.
	+ Multiple tasks can reside on the same physical machine or across different machines
	+ Task exchange data through communications by sending  and receiving messages
	+ Implementation:
		. Message Passing Interface (MPI)
- Hybrid model
4. Design Parallel Program
- Approach:
	+ Study the problem to see whether or not it can be solved by parallel programs
	+ Study existing serial programs to see if it can be parallelized
	+ Investigate other algorithms
	+ Employ optimized parallel softwares or math libraries from leading vendors
- Partitioning
	+ Break the problem into chunks of works that can be distributed to multiple tasks
		. Domain decomposition
		. Functional decomposition
	+ Domain decomposition
		. 1D: block, cyclic,...
		. 2D: (block, \*), (\*, block), (block, block), (cyclic, \*), (\*, cyclic), (cyclic, cyclic),...
- Communication
- Synchronization
	+ Barrier
	+ Lock/ semaphore
	+ Synchronization communication
- Load balancing
5. Computing platform
- PC
	+ Multiple CPUs
	+ Multiple cores per CPU
	+ Multiple threads per core
- Super computer
	+ Multiple CPUs
- Cluster/Cloud/Grid
	+ Multiple PCs
- GPGPU
	+ Multiple threads
6. High performance computing problems

# OpenMP
1. Introduction
- OpenMP is an API allow build application on shared memory system in C, C++ and Fortran. Include:
	+ Compiler directives
	+ Runtime Library rountines
	+ Enviroment variables
- Syntax:
	+ `include<omp.h>`
	+ `#pragma omp parallel clause`. List of clause:
		. `if (scalar_expression)`
		. `private (list)`
		. `shared (list)`
		. `default (shared || none)`
		. `firstprivate(list)`
		. `reduction(operator: list)`
		. `copyin (linst)`
		. `num_threads(integer-expression)`
		. Ex: `#pragma omp parallel`
	+ `omp_get_thread_num()`
	+ `omp_get_num_threads()`
- Compile
	+ `gcc -fopenmp hw.c`
2. Fork/join model
- OpenMP programs start with a single thread (master thread #0)
- At start of parallel region master creates team of parallel "worker" threads (Fork)
- Statements in parallel block are execute in parallel by every thread
- At the end of parallel region, all threads synchronize and join master thread (Join)
3. OpenMP Thread vs CPU Core
- Mapping n threads to 1 core
- In practice, mapping 1-1 is the best choice
- Set number of OpenMP threads:
	+ Enviroment variable `OMP_NUM_THREADS`
	+ Function `omp_set_num_threads(int n)`
- Get threads information
	+ Number of threads: `int omp_get_num_threads()`
	+ Id of thread: `int omp_get_thread_num()`
4. Shared memory model
- All threads try access the same variables
- Shared and Private variables:
	+ `SHARED (list)`
		. Ex: `#pragma omp parallel shared(id,x)`
	+ `PRIVATE (list)`
		. Ex: `#pragma omp parallel private(id,x)`
	+ By default most variables are considered shared in OpenMP, except include index variable (Fortran, C/C++) and variables declared inside parallel region (C/C++)
	+ OpenMP creates separated data stacks for every worker thread to store copies of private variables (master thread uses regular stack)
	+ Size of these stacks defined by compiler:
		. Intel compiler: 4MB default
		. gcc/gfortran: 2MB default
		. `export OMP_STACKSIZE=512MB`
		. `export OMP_STACKSIZE=1GB`
		. `ulimit -s command` (unix/linux) to unlimit master thread stack size

# MPI
1. Compile
- `mpicc hw.c`
- `mpic++ hw.cpp`
2. Run
- `mpirun -np num_of_processes hw`
- `mpirun -H localhost:32 -np 32 ./b1`
- `mpirun --hostfile hostfile -np 32 ./b1`
- Host file:
	+ each line is an address of a host in the system
3. MPI Routines
- `int MPI_Init(int *argc, char ***argv)`
	+ Return MPI_SUCCESS or an error code
	+ This function initializes the MPI execution enviroment
	+ This function must be called by each MPI process before any MPI function is executed
- `int MPI_Finalize()`
	+ Free any resources
	+ Each MPI process must call this function before exiting
- `int MPI_Comm_size(MPI_Comm comm, int *size)`
	+ IN comm: Communicator
	+ OUT size: number of processes in the communication group
	+ Communicator: Indentifies a process group and defines the communication context. All message tags are unique with respect to a communication
	+ MPI_COMM_WORLD: The processes group includes all processes of a parallel application
	+ MPI_Comm_size: returns the number of processes in the group of the given communicator
- `int MPI_Comm_rank(MPI_Comm comm, int *rank)`
	+ IN comm: Communicator
	+ OUT rank: id of the process in the communication group
	+ MPI_Comm_rank: returns the id of process in the group of the given communicator
- `void MPI_Send(void *buf, int count, MPI_Datatype dtype, int dest, int tag, MPI_Comm comm)`
	+ IN buf: send buffer
	+ IN count: number of items to be sent
	+ IN dtype: type of the items
	+ IN dest: receiver id
	+ IN tag: message tag
	+ IN comm: Communicator
	+ MPI_Send is a blocking function
		. Terminates when the send buffer can be reused
			. the message was delivered
			. the data were copied to a system buffer
- `void MPI_Recv(void *buf, int count, MPI_Datatype dtype, int source, int tag, MPI_Comm comm, MPI_Status *status)`
	+ IN buf: receive buffer
	+ IN count: number of items to be received
	+ IN dtype: type of the items
	+ IN dest: Sender id
	+ IN tag: message tag
	+ IN comm: Communicator
	+ OUT status: the status information
	+ MPI_Recv is a blocking function
		. It terminates when the message is available in the receiver buffer
		. Message length must <= receiver buffer
			. The remaining part of the buffer will be unchanged
		. Message to be received must math the sender, the tag and the communicator
			. Sender and tag can be specified as wild card: MPI_ANY_SOURCE and MPI_ANY_TAG
		. Actual length of the received message:
			. `int MPI_Get_count(MPI_Status *status, MPI_Datatype datatype, int *count)`
				. OUT count: number of received items
- `void MPI_Isend(void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request)`
	+ OUT request: communication request (handle)
- `void MPI_Irecv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request)`
- `int MPI_Wait(MPI_Request *request, MPI_Status *status)`
- `MPI_Sendrecv(void *sendbuf, int sendcount, MPI_Datatype sendtype, int dest, int sendtag, void *recvbuf, int recvcount, MPI_Datatype recvtype, int source, int recvtag, MPI_Comm comm, MPI_Status stat)`
	+ Equivalent to the execution of MPI_Send and MPI_Recv in parallel threads
	+ sendbuf and recvbuf are different buffers
- `MPI_Bcast(void *sendbuf, int count, MPI_Datatype type, int root, MPI_Comm comm)`
	+ The content of sendbuf of the process root is copied to all other process
	+ Blocking function
- `MPI_Gather(void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm)`
	+ Process root receives the data in the send buffer of all processes
	+ The received data is stored in the receive buffer ordered by the process id of the senders
	+ `recvcount` is the number of items to be received from each process
- `MPI_Scatter(void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm)`
	+ Process root seeds the data in the sendbuf to all other processes in the communicator `comm`
	+ `sendcount` is the number of items to be sent to each process
	+ `sendbuf` is divided into chunks of `sendcount` items
	+ `recvcount` is the number of items to  be received for each process
- `MPI_Reduce(void *sendbuf, void *recvbuf, int sendcount, MPI_Datatype sendtype, MPI_Op op, int root, MPI_Comm comm)`
	+ Reduces values of all processes to a single process root
	+ `op` is the operator:
		. MPI_MAX
		. MPI_MIN
		. MPI_SUM
		. MPI_PROD
		. ...

# CUDA
1. What is CUDA?
- CUDA Architecture
	+ Expose GPU computing for general purpose
	+ Retain performance
- CUDA C/C++
	+ Based on industry-standard C/C++
	+ Small set of extensions to enable hetorogeneous programming
	+ Straightforward APIs to manage devide, memory,...

2. Concepts
- Heterogeneous Computing
	+ `Host`: CPU and its memory <-> `host code`
	+ `Device`: GPU and its memory <-> `device code`
	+ Simple Processing Flow
		. Copy input data from CPU memory to GPU memory
		. Load GPU code and execute it, caching data on chip for performance
		. Copy result from GPU memory to CPU memory
- Standard C that runs on the host
- NVIDIA compiler (nvcc) can be used to compile programs with no device code
	+ Keyword \_\_global\_\_ indicates a function that:
		. Runs on the device
		. Is called from host code
	+ Device functions processed by NVIDIA compiler
	+ Host functions processed by standard host compiler (gcc, cl.exe,...)
	+ Triple angle brackets (Ex: `<<<N, M>>>`):
		. Mark a call from `host` code to `device` code (Also called a "kernel launch")
		. (N, M) are respectively number of blocks and number of threads per block.
	+ Device variables must be stored in device memory
		. Use `cudaMalloc()`, `cudaFree()`, `cudaMemcpy()` (similar to `malloc()`, `free()`, `memcpy()`)
		. Can alloc device memory in host code
	+ Block: Physical unit
		. blockIdx.x: index of block
	+ Thread: Virtual unit
		. threadIdx.x: index of thread
	+ Thread > Block: Communicate and Synchronize
		. Within a block, threads share data via shared memory
		. Declare data using `__shared__`
		. Data not visible to threads in other blocks
	+ Synchronize all threads within a block
		. To prevent RAW/WAR/WAW hazard
		. All threads must reach the barier. Use `__syncthreads()`;
- Race conditions
	+ A `race condition` occurs when program behavior depends upon relative timing of two (or more) event sequences
	+ `Atomic Operation`:
		. `atomicAdd()`
		. `atomicSub()`
		. `atomicMax()`
		. `atomicMin()`
		. `atomicInc()`
		. `atomicDec()`
		. `atomicExch()`
		. `atomicCAS()`
- Managing the device
	+ Kernel launch are asynchronous
		. Control returns to the CPU immediately
	+ CPU need to synchronize before consuming the result:
		. `cudaMemcpy()`: Block the CPU until the copy is complete. Copy begins when all preceding CUDA calls have completed.
		. `cudaMemcpyAsync()`: Asynchronous, does not block the CPU.
		. `cudaDeviceSynchronize()`: Blocks the CPU until all preceding CUDA calls have completed.
- Reporting Error
	+ `cudaError_t cudaGetLastErrod(void)`
	+ `char *cudaGetErrorString(cudaError_t)`
- Device Management
	+ `cudaGetDeviceCount(int *count)`
	+ `cudaSetDevice(int device)`: select current device
	+ `cudaGetDevice(int *device)`
	+ `cudaGetDeviceProperties(cudaDeviceProp *prop, int device)`
	+ Multiple host threads can share a device
	+ A single host thread can manage multiple devices
	+ `cudaMemcpy()`: p2p copies