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

//Before we start, it is important that you read the Introduction to Inline Assembly Tutorial, so you have a basic understanding of assembly
	//It is important, so you can handle/check memory reordering in a real time context

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

	//How variables are stored

	//Used: https://msdn.microsoft.com/en-us/library/windows/desktop/ee418650(v=vs.85).aspx

	//Preventing Compiler and CPU Reordering
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
		//http://preshing.com/20120612/an-introduction-to-lock-free-programming/

	//http://stackoverflow.com/questions/1147904/x86-equivalent-for-lwarx-and-stwcx

	//What are acquire and release semantics?
	//How to use the atomic library?
	//When to use fences?

	//For debugging: http://preshing.com/20120522/lightweight-in-memory-logging/

	//To have basic data types that are guaranteed to be a certain size, use: <cstdint>

	Atomics a;
	a.examples();

	//LockedSPSCQueue queue(0);
	SPSCQueue queue(0);

	std::thread consumer(func1, std::ref(queue));
	std::thread producer(func2, std::ref(queue));
	/*std::thread consumer(func1L, std::ref(queue));
	std::thread producer(func2L, std::ref(queue));*/

	consumer.detach();
	producer.detach();


	while (!isDone1 || !isDone2){};

	Node* front = queue.front();
	Node* back = queue.back();



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
	for (int i = 0; i < q.size(); ++i)
	{
		printf("Dequeued: %d\n", q.dequeue());
	}

	//The below is merely for testing purposes
	while (!isDone1){}
	
	int size = q.size();
	for (int i = 0; i < size; ++i)
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