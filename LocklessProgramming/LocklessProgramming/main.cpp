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

//Used:
	//https://www.youtube.com/watch?v=lVBvHbJsg5Y
		//for barriers - 25 min
	//https://www.youtube.com/watch?v=1obZeHnAwz4
	//http://www.cplusplus.com/reference/atomic/atomic/
		//for the atomic definition
	//https://msdn.microsoft.com/en-us/library/windows/desktop/ee418650(v=vs.85).aspx
	//http://preshing.com/20120612/an-introduction-to-lock-free-programming/


//Before we start, it is important that you read the Introduction to Inline Assembly Tutorial, so you have a basic understanding of assembly
	//It is important, so you can handle/check memory reordering in a real time context

//Don't use lockless programming unless you NEED the performance and the scalability
	//test performance before you use lockless to make sure
		//test performance before and after to make sure you actually got the performance boost you wanted

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
	//Read-Acquire and Write-Release Barriers/Semantics

//Need to work on this!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//Lockless or Lock-free vs Wait-free vs Blocking vs Non-Blocking
//You will come across these 3 terms if you research lockless programming
	//Used almost : https://www.justsoftwaresolutions.co.uk/threading/non_blocking_lock_free_and_wait_free.html
	//Blocking - those algorithms or data structures that calls an operating system function that waits for an event to occur or a time period to elapse
	//Non-Blocking - just those algorithms or data structures that don't block, all lock-free data structures or algorithms are Non-Blocking
	//Lockless or Lock-free: if any thread that performs an operation on the data structure is suspended at any point during that operation then the other threads accessing the data structure must still be able to complete their tasks
	//Wait-free: A data structure or algorithm that is lock-free and it can complete its operation in a bounded number of steps
	//For the rest of this tutorial I will not talk about these, but it is good to know the differences

void func1(SPSCQueue&);
void func2(SPSCQueue&);
void func1L(LockedSPSCQueue&);
void func2L(LockedSPSCQueue&);

bool isDone1 = false;
bool isDone2 = false;

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
	
	//Used: https://msdn.microsoft.com/en-us/library/windows/desktop/ee418650(v=vs.85).aspx

	
	
	//What are acquire and release semantics?
	//How to use the atomic library?
	//When to use fences?


	//To have basic data types that are guaranteed to be a certain size, use: <cstdint>
	
	Atomics a;
	a.examples();

	//Now that we have gone over atomics and the basics of memory ordering with atomics, it is time to talk about Read-Modify-Write operations
	//Dealing With Read-Modify-Write Operations
		//http://preshing.com/20150402/you-can-do-any-kind-of-atomic-read-modify-write-operation/

	LockedSPSCQueue queue(0);
	//SPSCQueue queue(0);

	/*std::thread producer(func1, std::ref(queue));
	std::thread consumer(func2, std::ref(queue));*/
	std::thread consumer(func1L, std::ref(queue));
	std::thread producer(func2L, std::ref(queue));

	producer.detach();
	consumer.detach();
	


	while (!isDone1 || !isDone2){};

	//Node* front = queue.front();
	//Node* back = queue.back();



	getchar();
	return 0;
}

void func1(SPSCQueue& q)
{
	for (int i = 0; i < 90; i++)
	{
		q.enqueue(i+1);
	}
	isDone1 = true;
}
void func2(SPSCQueue& q)
{
	for (int i = 0; i < 91; ++i)
	{
		printf("Dequeued: %d\n", q.dequeue());
	}
	isDone2 = true;
	printf("Done!");
}
void func1L(LockedSPSCQueue& q)
{
	for (int i = 0; i < 1000; ++i)
	{
		q.enqueue(i+1);
	}
}
void func2L(LockedSPSCQueue& q)
{
	for (int i = 0; i < 100; ++i)
	{
		printf("Dequeued: %d\n", q.dequeue());
		//std::this_thread::sleep_for(std::chrono::microseconds(1));
	}
	printf("Done!");
}

//Here is a resource for debugging multithreaded code:
	//http://preshing.com/20120522/lightweight-in-memory-logging/