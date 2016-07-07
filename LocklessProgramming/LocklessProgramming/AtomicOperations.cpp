//Used:
	//https://en.wikipedia.org/wiki/Linearizability

//What are Atomic Operations
	//it is atomic if it appears to the rest of the system to occur instantaneously
	//A guarantee of isolation from concurrent processes

//say we have an integer p
int p;

//An example of an atomic operation
	//merely stores 10 into p
//p = 10;

//An example of an operation that isn't atomic
	//loads p, increments it, and then stores that number in p
//p++;

//Game Atomic Library - low level library
	//Has a way to declare atomic variables
	//has a way to do atomic loads and atomic stores
	//has a way to enforce memory ordering
		//a lightweight fence - orders loads from memory and orders stores to memory
			//Used way more often
			//Can actually be done witha a compiler barrier
			//does the job of these in C++11
				//atomic_thread_fence(memory_order_acquire);
				//atomic_thread_fence(memory_order_release);
				//atomic_thread_fence(memory_order_acq_rel);
		//a full fence - does everything a lightweight fence does, but it commits all previous stores to memory before beginning the next load
			//does the job of this in C++11:
				//atomic_thread_fence(memory_order_seq_cst);
	//has a portable set of read-modify-write operations