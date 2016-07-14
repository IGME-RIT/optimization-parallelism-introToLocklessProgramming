//What is the ABA problem?

//preemption - the act of temporarily interrupting a task being carried out by a computer system
				//and with the intention of resuming the task at a later time
	//https://en.wikipedia.org/wiki/Preemption_(computing)

//There are two major problems that you will encounter with lockless programming
	//Data Races
	//The ABA problem

//We will cover Data Races first, since there isn't too much to talk about
	//We have gone over Data Races before in the Multithreaded Issues tutorial
	//This is the same thing, it is just a more prevalent issue, since you aren't locking things
	//I am bringing this up so you are aware of it and I'm not going to go any further with it

//The other of the major problems that occurs in Lockless Programming is the ABA problem.
	//https://en.wikipedia.org/wiki/ABA_problem
	//This is when multiple threads or processes accessing shared memory interleave
		//Thread T1 reads value A from shared memory
		//T1 is preempted allowing thread T2 to run
		//T2 modifies the shared memory value A to value B and back to A before preemption
		//T1 begins execution again, sees that the shared memory value has not changed and continues
	//This is a major problem in CAS loops, since the CAS can't detect this behavior
	//This is very important to keep in mind and make sure it can't happen

	//Solution to the ABA Problem
		//s

//One final warning,
	//Be very careful with Lockless Programming,
		//since there are numerous stories of writing lockless code and it working fine for a while until the programmers happen upon a data race a month later or something