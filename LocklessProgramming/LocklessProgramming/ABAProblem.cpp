//What is the ABA problem?

//preemption - the act of temporarily interrupting a task being carried out by a computer system
				//and with the intention of resuming the task at a later time
	//https://en.wikipedia.org/wiki/Preemption_(computing)


//https://en.wikipedia.org/wiki/ABA_problem
//When multiple threads or processes accessing shared memory interleave
//Thread T1 reads value A from shared memory
//T1 is preempted allowing thread T2 to run
//T2 modifies the shared memory value A to value B and back to A before preemption
//T1 begins execution again, sees that the shared memory value has not changed and continues