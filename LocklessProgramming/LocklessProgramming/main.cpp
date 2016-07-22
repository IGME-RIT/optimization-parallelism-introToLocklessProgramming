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
//One other thing before we start is that just like all the other tutorials, we are assuming that you are using Visual Studio 2013 and its compiler	

//Don't use lockless programming unless you NEED the performance and the scalability
	//test performance before you use lockless to make sure
	//Double check if there is another way to do it without using lockless programming
		//Can you change your code to have less contension on the mutexes or speed it up in another way?
	//test performance after to make sure you actually got the performance boost you wanted or needed
		

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

int main()
{
	//What is lock-Free programming?
		//Part of it is just programming without mutexes
		//The broader definition is programming in such a way to prevent the possibility of "locking up" the entire application in some way
		//http://preshing.com/20120612/an-introduction-to-lock-free-programming/

	//Should you use lockless programming?
		//Not unless you absolutely need the performance
		//Lockless is very complex and requires a good understand of the cache, your architecture, and other things
		//A lot of the time using mutexes can be much easier
	
	//What is good about Lockless Programming?
		//if done well, it can be very quick
		//Does not have most of the issues that come from using mutexes (deadlock, livelock, etc.)

	//Two Things We Need To Pay Attention to When Doing Lockless Programming
		//Non-Atomic Operations
		//Memory Reordering
	
	//Used: https://msdn.microsoft.com/en-us/library/windows/desktop/ee418650(v=vs.85).aspx

	//To have basic data types that are guaranteed to be a certain size, use: <cstdint>
	

	//Now lets start with std::atomics
	Atomics a;
	a.examples();

	//Now lets go to MemoryOrderWithAtomics.cpp

	//Now that we have gone over atomics and the basics of memory ordering with atomics, it is time to talk about Read-Modify-Write operations
	//Dealing With Read-Modify-Write Operations
		//http://preshing.com/20150402/you-can-do-any-kind-of-atomic-read-modify-write-operation/

	//Now lets talk about how to go about creating lockless code
	//Well, in this tutorial or primary means of writing lockless programming will be the standard library atomic objects and smart programming
		//this tutorial will assume you are not using volatile or memory fences, since standard library atomics deal with the same issues those deal
	//But, we'll get to that later
	//A lot of the time people create lockless code by creating lockless data structures that handle operations between threads
	//Lets use a queue to discuss this, obviously the queue is an example
		//You create the queue and make it so that its various functions control the queue
		//What are various queue functions:
			//enqueue
			//dequeue
			//size <- just a getter for the size of the queue
		//Those are the basic functions and any others would be custom to your implementation
		//Now what functions could there possibly be contension issues?
			//Now in some data structures only certain functions could possibly cause contention
			//In this case though, all functions have a possibility of creating tension
		//Lets talk through each function and talk about possible places of contention
			//size
				//the contention comes from the fact that the internal size variable could be changed by both the enqueue and the dequeue functions
			//enqueue
				//Now for this there are a few places of contention
					//adding to the queue inherently can involve cases of contention
						//Now this really depends on the implementation of your queue and what restrictions you put on the threads' use of this function
							//Example of implementations: your queue can internally be an array or maybe a singly linked list
							//Example of restrictions: only one thread may ever enqueue
						//Example of contention: two threads try to enqueue at the same time
					//editing the classes internal size variable, since this can be accessed and changed by other functions
			//dequeue
				//Now for this there are a few places of contention
					//deleting from the queue inherently can involve cases of contention
						//Now this really depends on the implementation of your queue and what restrictions you put on the threads' use of this function
							//Example of implementations: your queue can internally be an array or maybe a singly linked list
							//Example of restrictions: only one thread may ever enqueue
						//Example of contention: two threads try to dequeue at the same time
					//editing the classes internal size variable, since this can be accessed and changed by other functions

	//Now see how many places there could be contention it is time to talk about what to do about it
	//Now that we know where the contention is the next thing is to see what variables we need to make atomic	<- This is where we start assuming the use of standard library atomics
		//These will tend to be the variables that will be used in multiple functions in the data structure
			//It also depends on if there reads and writes are atomic
			//Also depends on whether you need to prevent memory reordering of the variable
	//Once you know what variables are needed to be atomic, make it atomic and apply the appropriate memory ordering to the various atomic functions (fetch_add(), exchange(), etc.)
	//Now here is where I can't truly give a concrete step but more of a consideration
		//Consider where you can make temporary variables to mess with instead of dealing with the atomic variable
		//This is important, since you can make changes to the temporary variable without affecting the original
		//Then when you want to change the original variable, you can use the compare_exchange_strong()

	//Now before we continue to an example of a Lockless Queue, look at the LocklessProblems.cpp

	//Finally lets show an example of a Lockless Queue
	//I have also created a similar version of the lockless queue that uses locks

	SPSCQueue queue(0);
	//LockedSPSCQueue queue(0);

	std::thread producer(func1, std::ref(queue));
	std::thread consumer(func2, std::ref(queue));
	/*std::thread consumer(func1L, std::ref(queue));
	std::thread producer(func2L, std::ref(queue));*/

	producer.join();
	consumer.join();
	
	getchar();
	return 0;
}

void func1(SPSCQueue& q)
{
	for (int i = 0; i < 90; i++)
	{
		q.enqueue(i+1);
	}
}
void func2(SPSCQueue& q)
{
	for (int i = 0; i < 91; ++i)
	{
		printf("Dequeued: %d\n", q.dequeue());
	}
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

//This is it. This gives the basics for Lockless multithreaded code using standard library atomics.
//Now one last point. This is not all there is to Lockless Programming. It is honestly just the surface.
//Remember Lockless Programming is the use of specific programming techniques and lower level synchronization objects to prevent code from locking up your program.
	//This includes memory barriers, semaphores, atomics, and more.