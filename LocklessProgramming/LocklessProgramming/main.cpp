/*
Lockless Programming
(c) 2016
Original Authors: Sean Connolly
Written under the supervision of David I. Schwartz, Ph.D., and
supported by a professional development seed grant from the B. Thomas
Golisano College of Computing & Information Sciences
(https://www.rit.edu/gccis) at the Rochester Institute of Technology.
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at
your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

Description:
*	This tutorial deals with the techniques and concepts of lockless programming.
*	We will also go over the meaning of lockless programming, the various forms it takes, and finally when to use it.
*/

#include "ClassDeclarations.h"
#include "SinglyLinkedListAtomics.h"

//Used:
	//https://www.youtube.com/watch?v=lVBvHbJsg5Y
		//for barriers - 25 min
	//https://www.youtube.com/watch?v=1obZeHnAwz4
	//http://www.cplusplus.com/reference/atomic/atomic/
		//for the atomic definition
	//https://msdn.microsoft.com/en-us/library/windows/desktop/ee418650(v=vs.85).aspx
	//http://preshing.com/20120612/an-introduction-to-lock-free-programming/

//Read:
	//https://www.infoq.com/news/2014/10/cpp-lock-free-programming
	//http://stackoverflow.com/questions/25199838/understanding-stdatomiccompare-exchange-weak-in-c11
	//http://www.drdobbs.com/lock-free-data-structures/184401865
	//http://www.cplusplus.com/reference/atomic/atomic/
	//http://preshing.com/20111203/a-c-profiling-module-for-multithreaded-apis/


//https://en.wikipedia.org/wiki/ABA_problem
//http://www.gamedev.net/topic/667092-the-atomic-man-are-lockless-data-structures-really-worth-learning-about/

//Don't use lockless programming unless you NEED the performance and the scalability
	//test performance before you use lockless to make sure
//test performance before and after

//Order of Topics to Cover
	//Atomic Operations - store, load and other things
	//Atomics
		//compare_exchange_strong(), compare_exchange_weak(), load(), store(), exchange()
	//Lockless Programming in general
		//Theory
		//Algorithms and Data Structures
		//needs work
	//Lockless Data Structures
		//needs work
	//Reordering
	//Read-Acquire and Write-Release Barriers
	//Preventing Compiler Reordering
	//Preventing CPU reordering
	//Volatile Variables and Reordering

//Need to work on this!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//Lockless or Lock-free vs Wait-free
//You will come across these 3 terms if you research lockless programming
	//Lockless or Lock-free: Not using locks
	//Wait-free: Making sure that no thread is waiting to make progress
	//For the rest of this tutorial I will not point out the differences, since to truly due this right, you need to do both at the same time
	//It is good to know the differences

int main()
{
	//What is lock-Free programming?
		//Part of it is just programming without mutexes
		//The broader definition is programming in such a way to prevent the possibility of "locking up" the entire application in some way
		//http://preshing.com/20120612/an-introduction-to-lock-free-programming/

	//Should you use lockless programming?
		//Not unless you absolutely need the performance
		//Lockless is very complex and requires a good understand of the cache, your architecture, and other things

	//Two Things We Need To Pay Attention to When Doing Lockless Programming
		//Non-Atomic Operations
		//Reordering

	//Atomic Vs Non-Atomic Operations
		//A Atomic operation is an operation that takes a single cycle
			//A cycle or instruction cycle is the process by which a computer retrieves a program instruction from its memory, determines what actions the instructions requires and carries out those actions
				//https://en.wikipedia.org/wiki/Instruction_cycle
			//Example: num = 1;
				//This is a single store operation, thus making it atomic
		//A non-atomic operation takes more than a single cycle
			//Example: int temp = num + 1; num = temp;
				//This first has a load operation on num. Then it stores temp into num.
				//While the individual operations are atomic, the overall operation isn't atomic
		//num++ may or may not be an atomic operation depending on the architecture

	//How variables are stored

	//Reordering - Used: https://msdn.microsoft.com/en-us/library/windows/desktop/ee418650(v=vs.85).aspx
		//Reads and Writes do not always happen in the order that you have written them in your code
		//Write-Release
			//a thread writes some data and then writes to a flag that tells other threads that the data is ready
		//Read-Acquire
			//a thread reads from a flag and then reads some shared data if the flag says that the thread has acquired access to the shared data
		//reads and writes can be reordered by the compiler or the CPU
			//This doesn't appear on single threaded systems

	//Read-Acquire and Write-Release Barriers
	//Preventing Compiler Reordering
	//Preventing CPU reordering
	//Interlocked Functions and CPU Reordering
	//Volatile Variables and Reordering

	//Fences/Memory Barrier vs Semaphore vs Mutexes
		//http://stackoverflow.com/questions/10552085/difference-between-lock-memory-barrier-semaphore

	//Dealing With Read-Modify-Write Operations
		//http://preshing.com/20150402/you-can-do-any-kind-of-atomic-read-modify-write-operation/

	//For more fun:
		//http://preshing.com/20120930/weak-vs-strong-memory-models/
		//http://preshing.com/20140709/the-purpose-of-memory_order_consume-in-cpp11/
		//http://preshing.com/20120913/acquire-and-release-semantics/
		//http://preshing.com/20130823/the-synchronizes-with-relation/
		//http://cellperformance.beyond3d.com/articles/index.html

	//What are acquire and release semantics?
	//How to use the atomic library?
	//When to use fences?

	//For debugging: http://preshing.com/20120522/lightweight-in-memory-logging/

	//To have basic data types that are guaranteed to be a certain size, use: <cstdint>

	Atomics a;
	a.examples();

	//addToLL();

	getchar();
	return 0;
}